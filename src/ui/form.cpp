#include "form.h"
#include "ui_form.h"

/**
 * @brief Form::Form Constructor for the form class inits default GUI values
 * @param parent inherits from qwidget
 */
Form::Form(QWidget *parent) : QWidget(parent),
    ui(new Ui::Form),
    _redBrush(Qt::red),
    _yelBrush(Qt::yellow),
    _borderPen(Qt::black),
    _dashedPen(Qt::DashLine),
    _game_over(false),
    _winner(0)
{
    ui->setupUi(this);

    //model graphicsView/scene in it for the game field
    _scene = new QGraphicsScene(this);
    ui->graphicsView->setFixedSize(720, 620);
    ui->graphicsView->setSceneRect(-360, -310, 720, 620);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->fitInView(_scene->sceneRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setScene(_scene);

    // initialization method for general operations
    init();

    //fill positions array with zeros
    std::array<int, 6> dummy;
    dummy.fill(0);
    _positions.fill(dummy);

    //fill possibleDrops array for beginning
    _possibleDrops.resize(7);
    std::iota(_possibleDrops.begin(), _possibleDrops.end(), 0);

    //start the timer for GUI updates
    startTimer();
}

Form::~Form(){
    delete ui;
}

/**
 * @brief Form::init: Initialization method to create the default settings
 */
void Form::init(){
    //set the default depth (difficulty)
    ui->cmbb_difficulty_1->setCurrentIndex(3);
    ui->cmbb_difficulty_2->setCurrentIndex(3);
    _p1_depth = ui->cmbb_difficulty_1->currentIndex()+1;
    _p2_depth = ui->cmbb_difficulty_2->currentIndex()+1;

    //set if ai or human for both players (default settings)
    _p1_is_ai = false;
    ui->rdbtn_p1_ai->setChecked(false);
    ui->rdbtn_p1_human->setChecked(true);
    _p2_is_ai = true;
    ui->rdbtn_p2_ai->setChecked(true);
    ui->rdbtn_p2_human->setChecked(false);

    //set starting player
    _p_start = 1;
    ui->rdbtn_start_p1->setChecked(true);
    ui->rdbtn_start_p2->setChecked(false);

    //delete output list
    ui->lst_out->clear();

    //write start hint to scene
    QGraphicsTextItem * txtHelp = new QGraphicsTextItem;
    QFont fntHelp = txtHelp->font();
    fntHelp.setPixelSize(20);
    txtHelp->setFont(fntHelp);
    txtHelp->setPos(-150,0);
    txtHelp->setPlainText("Choose your settings and click start!");
    txtHelp->setTextWidth(400);
    _scene->addItem(txtHelp);

    //disable buttons at start
    ui->btn_drop_0->setDisabled(true);
    ui->btn_drop_1->setDisabled(true);
    ui->btn_drop_2->setDisabled(true);
    ui->btn_drop_3->setDisabled(true);
    ui->btn_drop_4->setDisabled(true);
    ui->btn_drop_5->setDisabled(true);
    ui->btn_drop_6->setDisabled(true);
}

/**
 * @brief Form::startTimer: GUI update timer is started
 */
void Form::startTimer(){
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGUI()));
    timer->start(50);
}

/**
 * @brief Form::on_btn_start_clicked: Starts the game
 */
void Form::on_btn_start_clicked()
{
    //write header of game to output
    ui->lst_out->addItem("new game\n");

    if(_p1_is_ai){
        writeToLog("P1: ai");
        writeToLog("Depth P1: " + QString::number(_p1_depth));
    }
    else{
        writeToLog("P1: human");
    }

    if(_p2_is_ai){
        writeToLog("P2: ai");
        writeToLog("Depth P2: " + QString::number(_p2_depth));
    }
    else{
        writeToLog("P2: human");
    }

    writeToLog("P" + QString::number(_p_start) + " starts\n");


    //prepare gui for game, clear scene, enable buttons
    ui->btn_start->setDisabled(true);
    _scene->clear();
    makelines();
    ui->btn_drop_0->setEnabled(true);
    ui->btn_drop_1->setEnabled(true);
    ui->btn_drop_2->setEnabled(true);
    ui->btn_drop_3->setEnabled(true);
    ui->btn_drop_4->setEnabled(true);
    ui->btn_drop_5->setEnabled(true);
    ui->btn_drop_6->setEnabled(true);

    //actually generate game and start it
    game = new Game(this, _p1_is_ai, _p2_is_ai, _p1_depth, _p2_depth, _p_start);
    game->start();

//    std::thread t(&Game::start, game);
//    if(t.joinable())
//        t.join();
}

/**
 * @brief Form::on_btn_reset_clicked: Reset all parameters and GUI
 */
void Form::on_btn_reset_clicked()
{
    //clear output list
    ui->lst_out->clear();

    //set back board variables of form and positions array
    _game_over = false;
    _winner = 0;

    //disable buttons at reset
    ui->btn_drop_0->setDisabled(true);
    ui->btn_drop_1->setDisabled(true);
    ui->btn_drop_2->setDisabled(true);
    ui->btn_drop_3->setDisabled(true);
    ui->btn_drop_4->setDisabled(true);
    ui->btn_drop_5->setDisabled(true);
    ui->btn_drop_6->setDisabled(true);

    // clear drawing scene
    _scene->clear();
    _scene->setBackgroundBrush(Qt::white);

    //fill positions array with zeros
    std::array<int, 6> dummy;
    dummy.fill(0);
    _positions.fill(dummy);

    //fill possibleDrops array for beginning
    _possibleDrops.resize(7);
    std::iota(_possibleDrops.begin(), _possibleDrops.end(), 0);

    //disable start button
    ui->btn_start->setDisabled(false);

    //write hint to scene
    QGraphicsTextItem * txtHelp = new QGraphicsTextItem;
    QFont fntHelp = txtHelp->font();
    fntHelp.setPixelSize(20);
    txtHelp->setFont(fntHelp);
    txtHelp->setPos(-150,0);
    txtHelp->setPlainText("Choose your settings and click start!");
    txtHelp->setTextWidth(400);
    _scene->addItem(txtHelp);
}

/**
 * @brief Form::save_drop: Preliminary checks of validity for a human drop. If valid, carry out with game object
 * @param pos position [0,6] to drop the coin
 */
void Form::save_drop(int pos){
    //check for validity
    if(((game->get_current_player() == 1 && !_p1_is_ai) || (game->get_current_player() == 2 && !_p2_is_ai)) && !game->game_over){
        bool flag = false;
        for(auto i: _possibleDrops){
            if(i == pos){
                flag=true;
            }
        }
        if(flag){//actually carry out the move
//            std::thread t(&Game::human_move, game, pos);
//            t.detach();
            game->human_move(pos);
        }
        else{//don't execute the move and write this to output list
            writeToLog("P" + QString::number(game->get_current_player()) + " : " + QString::number(pos) + " - invalid");
            writeToLog("------------------");
        }
    }
}

//drop button slots
void Form::on_btn_drop_0_clicked(){save_drop(0);}
void Form::on_btn_drop_1_clicked(){save_drop(1);}
void Form::on_btn_drop_2_clicked(){save_drop(2);}
void Form::on_btn_drop_3_clicked(){save_drop(3);}
void Form::on_btn_drop_4_clicked(){save_drop(4);}
void Form::on_btn_drop_5_clicked(){save_drop(5);}
void Form::on_btn_drop_6_clicked(){save_drop(6);}

//game setting slots
void Form::on_rdbtn_p1_ai_toggled(bool checked){_p1_is_ai = checked;}
void Form::on_rdbtn_p2_ai_toggled(bool checked){_p2_is_ai = checked;}
void Form::on_rdbtn_p1_human_toggled(bool checked){_p1_is_ai = !checked;}
void Form::on_rdbtn_p2_human_toggled(bool checked){_p2_is_ai = !checked;}
void Form::on_rdbtn_start_p1_toggled(bool checked){_p_start = checked? 1 : 2;}
void Form::on_rdbtn_start_p2_toggled(bool checked){_p_start = checked? 2 : 1;}
void Form::on_cmbb_difficulty_1_currentIndexChanged(int index){_p1_depth = index+1;}
void Form::on_cmbb_difficulty_2_currentIndexChanged(int index){_p2_depth = index+1;}

//draw the board
void Form::makelines(){
    //draw vertical lines of board
    _scene->addLine(-350,300,-350,-300,_borderPen);
    _scene->addLine(-250,300,-250,-300,_borderPen);
    _scene->addLine(-150,300,-150,-300,_borderPen);
    _scene->addLine(-50,300,-50,-300,_borderPen);
    _scene->addLine(50,300,50,-300,_borderPen);
    _scene->addLine(150,300,150,-300,_borderPen);
    _scene->addLine(250,300,250,-300,_borderPen);
    _scene->addLine(350,300,350,-300,_borderPen);

    //draw horizontal lines of board
    _scene->addLine(-350, -300, 350, -300, _dashedPen);
    _scene->addLine(-350, -200, 350, -200, _dashedPen);
    _scene->addLine(-350, -100, 350, -100, _dashedPen);
    _scene->addLine(-350, -000, 350, -000, _dashedPen);
    _scene->addLine(-350, 100, 350, 100, _dashedPen);
    _scene->addLine(-350, 200, 350, 200, _dashedPen);
    _scene->addLine(-350, 300, 350, 300, _dashedPen);
}

/*
 * Methods called by the observer as callback from game
 */
void Form::updatePositions(boardarray positions){
    //update positions from game
    _positions = positions;
}

void Form::updatePossibleDrops(std::vector<int> possibleDrops){
    //update possible drops from game
    _possibleDrops = possibleDrops;
}

void Form::writeToLog(QString item){
    //add list entry and scroll to bottom
    ui->lst_out->addItem(item);
    ui->lst_out->scrollToBottom();
}

void Form::gameOver(int winningPlayer){
    //update game over and winner parameter
    _game_over = true;
    _winner = winningPlayer;
}

void Form::setWinningLine(std::pair<std::pair<int, int>, std::pair<int, int>> winningLine){
    //set winning line for drawing
    _winner_line = winningLine;
}

/**
 * @brief Form::updateGUI: Called by timer event to update GUI
 */
void Form::updateGUI(){

    //draw the coins
    for(int i = 0; i < 7; ++i){
        for(int j = 0; j < 6; ++j){
            if(_positions[i][j] == 1){// 1 = yellow player
                _scene->addEllipse(-340 + (100*i), -290 + (100 * j), 80, 80, _borderPen, _yelBrush);
            }
            else if (_positions[i][j] == 2) {// 2 = red player
                _scene->addEllipse(-340 + (100*i), -290 + (100 * j), 80, 80, _borderPen, _redBrush);
            }
        }
    }

    //routine if game is over
    if(_game_over){

        //gray out background
        _scene->setBackgroundBrush(Qt::gray);

        //disable buttons at game over
        ui->btn_drop_0->setDisabled(true);
        ui->btn_drop_1->setDisabled(true);
        ui->btn_drop_2->setDisabled(true);
        ui->btn_drop_3->setDisabled(true);
        ui->btn_drop_4->setDisabled(true);
        ui->btn_drop_5->setDisabled(true);
        ui->btn_drop_6->setDisabled(true);

        if(_winner == 0){
            //write to scene
            QGraphicsTextItem * txtHelp = new QGraphicsTextItem;
            QFont fntHelp = txtHelp->font();
            fntHelp.setPixelSize(40);
            txtHelp->setFont(fntHelp);
            txtHelp->setPos(-50,-30);
            txtHelp->setPlainText("draw");
            txtHelp->setTextWidth(400);
            _scene->addItem(txtHelp);
        }
        else{
            //write to scene
            QGraphicsTextItem * txtHelp = new QGraphicsTextItem;
            QFont fntHelp = txtHelp->font();
            fntHelp.setPixelSize(40);
            txtHelp->setFont(fntHelp);
            txtHelp->setPos(-100,-30);
            txtHelp->setPlainText("player " + QString::number(_winner) + " won");
            txtHelp->setTextWidth(400);
            _scene->addItem(txtHelp);

            //define winning line (the 4 connected coins)
            int st_x = -300 + (100 * _winner_line.first.first);
            int st_y = -250 + (100 * _winner_line.first.second);
            int en_x = -300 + (100 * _winner_line.second.first);
            int en_y = -250 + (100 * _winner_line.second.second);
            QPen pen{10};

            //draw winning line
            _scene->addLine(st_x, st_y, en_x, en_y, pen);
        }
    }


}

