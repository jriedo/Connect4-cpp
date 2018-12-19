#include "form.h"
#include "ui_form.h"

/**
 * @brief Form::Form Constructor for the form class inits default GUI values
 * @param parent inherits from qwidget
 */
Form::Form(QWidget *parent) : QWidget(parent),
    ui(new Ui::Form),

    m_redBrush(Qt::red),
    m_yelBrush(Qt::yellow),
    m_borderPen(Qt::black),
    m_dashedPen(Qt::DashLine),
    m_game_over(false),
    m_winner(0)
{
    ui->setupUi(this);

    m_txtHelp.reset(new QGraphicsTextItem());

    //model graphicsView/scene in it for the game field
    m_scene = std::unique_ptr<QGraphicsScene>(new QGraphicsScene(this));
    ui->graphicsView->setFixedSize(720, 620);
    ui->graphicsView->setSceneRect(-360, -310, 720, 620);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setScene(m_scene.get());

    // initialization method for general operations
    init();

    //fill positions array with zeros
    std::array<int, 6> dummy;
    dummy.fill(0);
    m_positions.fill(dummy);

    //fill possibleDrops array for beginning
    m_possibleDrops.resize(7);
    std::iota(m_possibleDrops.begin(), m_possibleDrops.end(), 0);

    //start the timer for GUI updates
    startTimer();
}

/**
 * @brief Form::~Form   : destructor to delete ui
 */
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
    m_p1_depth = ui->cmbb_difficulty_1->currentIndex()+1;
    m_p2_depth = ui->cmbb_difficulty_2->currentIndex()+1;

    //set if ai or human for both players (default settings)
    m_p1_is_ai = false;
    ui->rdbtn_p1_ai->setChecked(false);
    ui->rdbtn_p1_human->setChecked(true);
    m_p2_is_ai = true;
    ui->rdbtn_p2_ai->setChecked(true);
    ui->rdbtn_p2_human->setChecked(false);

    //set starting player
    m_p_start = 1;
    ui->rdbtn_start_p1->setChecked(true);
    ui->rdbtn_start_p2->setChecked(false);

    //delete output list
    ui->lst_out->clear();

    //write start hint to scene
    //    m_txtHelp.reset(new QGraphicsTextItem());
    QFont fntHelp = m_txtHelp->font();
    fntHelp.setPixelSize(20);
    m_txtHelp->setFont(fntHelp);
    m_txtHelp->setPos(-150,0);
    m_txtHelp->setPlainText("Choose your settings and click start!");
    m_txtHelp->setTextWidth(400);
    m_scene->addItem(m_txtHelp.get());

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
    m_updateGuiTimer = std::unique_ptr<QTimer>(new QTimer(this));
    connect(m_updateGuiTimer.get(), SIGNAL(timeout()), this, SLOT(updateGUI()));
    m_updateGuiTimer->start(50);
}

/**
 * @brief Form::on_btn_start_clicked: Starts the game
 */
void Form::on_btn_start_clicked()
{
    //write header of game to output
    ui->lst_out->addItem("new game\n");

    if(m_p1_is_ai){
        writeToLog("P1: ai");
        writeToLog("Depth P1: " + QString::number(m_p1_depth));
    }
    else{
        writeToLog("P1: human");
    }

    if(m_p2_is_ai){
        writeToLog("P2: ai");
        writeToLog("Depth P2: " + QString::number(m_p2_depth));
    }
    else{
        writeToLog("P2: human");
    }

    writeToLog("P" + QString::number(m_p_start) + " starts\n");


    //prepare gui for game, clear scene, enable buttons
    ui->btn_start->setDisabled(true);
    m_scene->clear();
    makelines();
    ui->btn_drop_0->setEnabled(true);
    ui->btn_drop_1->setEnabled(true);
    ui->btn_drop_2->setEnabled(true);
    ui->btn_drop_3->setEnabled(true);
    ui->btn_drop_4->setEnabled(true);
    ui->btn_drop_5->setEnabled(true);
    ui->btn_drop_6->setEnabled(true);

    //actually generate game and start it
    m_game = std::unique_ptr<Game>(new Game(this, m_p1_is_ai, m_p2_is_ai, m_p1_depth, m_p2_depth, m_p_start));
    m_game->start();
}

/**
 * @brief Form::on_btn_reset_clicked: Reset all parameters and GUI
 */
void Form::on_btn_reset_clicked()
{
    //clear output list
    ui->lst_out->clear();

    //set back board variables of form and positions array
    m_game_over = false;
    m_winner = 0;

    //disable buttons at reset
    ui->btn_drop_0->setDisabled(true);
    ui->btn_drop_1->setDisabled(true);
    ui->btn_drop_2->setDisabled(true);
    ui->btn_drop_3->setDisabled(true);
    ui->btn_drop_4->setDisabled(true);
    ui->btn_drop_5->setDisabled(true);
    ui->btn_drop_6->setDisabled(true);

    // clear drawing scene
    m_scene->clear();
    m_scene->setBackgroundBrush(Qt::white);

    //fill positions array with zeros
    std::array<int, 6> dummy;
    dummy.fill(0);
    m_positions.fill(dummy);

    //fill possibleDrops array for beginning
    m_possibleDrops.resize(7);
    std::iota(m_possibleDrops.begin(), m_possibleDrops.end(), 0);

    //disable start button
    ui->btn_start->setDisabled(false);
}

/**
 * @brief Form::save_drop: Preliminary checks of validity for a human drop. If valid, carry out with game object
 * @param pos position [0,6] to drop the coin
 */
void Form::save_drop(int pos){
    //check for validity
    if(((m_game->get_current_player() == 1 && !m_p1_is_ai) || (m_game->get_current_player() == 2 && !m_p2_is_ai)) && !m_game->game_over){
        bool flag = false;
        for(auto i: m_possibleDrops){
            if(i == pos){
                flag=true;
            }
        }
        if(flag){//actually carry out the move
            //            std::thread t(&Game::human_move, m_game, pos);
            //            t.detach();
            m_game->human_move(pos);
        }
        else{//don't execute the move and write this to output list
            writeToLog("P" + QString::number(m_game->get_current_player()) + " : " + QString::number(pos) + " - invalid");
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
void Form::on_rdbtn_p1_ai_toggled(bool checked){m_p1_is_ai = checked;}
void Form::on_rdbtn_p2_ai_toggled(bool checked){m_p2_is_ai = checked;}
void Form::on_rdbtn_p1_human_toggled(bool checked){m_p1_is_ai = !checked;}
void Form::on_rdbtn_p2_human_toggled(bool checked){m_p2_is_ai = !checked;}
void Form::on_rdbtn_start_p1_toggled(bool checked){m_p_start = checked? 1 : 2;}
void Form::on_rdbtn_start_p2_toggled(bool checked){m_p_start = checked? 2 : 1;}
void Form::on_cmbb_difficulty_1_currentIndexChanged(int index){m_p1_depth = index+1;}
void Form::on_cmbb_difficulty_2_currentIndexChanged(int index){m_p2_depth = index+1;}

//draw the board
void Form::makelines(){
    //draw vertical lines of board
    m_scene->addLine(-350,300,-350,-300,m_borderPen);
    m_scene->addLine(-250,300,-250,-300,m_borderPen);
    m_scene->addLine(-150,300,-150,-300,m_borderPen);
    m_scene->addLine(-50,300,-50,-300,m_borderPen);
    m_scene->addLine(50,300,50,-300,m_borderPen);
    m_scene->addLine(150,300,150,-300,m_borderPen);
    m_scene->addLine(250,300,250,-300,m_borderPen);
    m_scene->addLine(350,300,350,-300,m_borderPen);

    //draw horizontal lines of board
    m_scene->addLine(-350, -300, 350, -300, m_dashedPen);
    m_scene->addLine(-350, -200, 350, -200, m_dashedPen);
    m_scene->addLine(-350, -100, 350, -100, m_dashedPen);
    m_scene->addLine(-350, -000, 350, -000, m_dashedPen);
    m_scene->addLine(-350, 100, 350, 100, m_dashedPen);
    m_scene->addLine(-350, 200, 350, 200, m_dashedPen);
    m_scene->addLine(-350, 300, 350, 300, m_dashedPen);
}

/*
 * Methods called by the observer as callback from game
 */
void Form::updatePositions(boardarray positions){
    //update positions from game
    m_positions = positions;
}

void Form::updatePossibleDrops(std::vector<int> possibleDrops){
    //update possible drops from game
    m_possibleDrops = possibleDrops;
}

void Form::writeToLog(QString item){
    //add list entry and scroll to bottom
    ui->lst_out->addItem(item);
}

void Form::gameOver(int winningPlayer){
    //update game over and winner parameter
    m_game_over = true;
    m_winner = winningPlayer;
}

void Form::setWinningLine(std::pair<std::pair<int, int>, std::pair<int, int>> winningLine){
    //set winning line for drawing
    m_winner_line = winningLine;
}

/**
 * @brief Form::updateGUI: Called by timer event to update GUI
 */
void Form::updateGUI(){

    ui->lst_out->scrollToBottom();
    //draw the coins
    for(int i = 0; i < 7; ++i){
        for(int j = 0; j < 6; ++j){
            if(m_positions[i][j] == 1){// 1 = yellow player
                m_scene->addEllipse(-340 + (100*i), -290 + (100 * j), 80, 80, m_borderPen, m_yelBrush);
            }
            else if (m_positions[i][j] == 2) {// 2 = red player
                m_scene->addEllipse(-340 + (100*i), -290 + (100 * j), 80, 80, m_borderPen, m_redBrush);
            }
        }
    }

    //routine if game is over
    if(m_game_over){
        m_game = nullptr;
        //gray out background
        m_scene->setBackgroundBrush(Qt::gray);

        //disable buttons at game over
        ui->btn_drop_0->setDisabled(true);
        ui->btn_drop_1->setDisabled(true);
        ui->btn_drop_2->setDisabled(true);
        ui->btn_drop_3->setDisabled(true);
        ui->btn_drop_4->setDisabled(true);
        ui->btn_drop_5->setDisabled(true);
        ui->btn_drop_6->setDisabled(true);

        if(m_winner != 0){
            //define winning line (the 4 connected coins)
            int st_x = -300 + (100 * m_winner_line.first.first);
            int st_y = -250 + (100 * m_winner_line.first.second);
            int en_x = -300 + (100 * m_winner_line.second.first);
            int en_y = -250 + (100 * m_winner_line.second.second);
            QPen pen{10};

            //draw winning line
            m_scene->addLine(st_x, st_y, en_x, en_y, pen);
        }
    }


}

