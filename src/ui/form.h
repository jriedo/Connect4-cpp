#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QString>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsItem>

#include "board.h"
#include "game.h"




namespace Ui {
class Form;
}

/**
 * @brief The Form class contains all handles for the GUI and is the only class interacting with it
 */
class Form : public QWidget, public Observer
{
    Q_OBJECT
    using boardarray = std::array<std::array<int, 6>, 7>;

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form() override;


private:
    Ui::Form* ui;
    std::unique_ptr<QGraphicsScene> m_scene;
    std::unique_ptr<Game> m_game;
    std::unique_ptr<QTimer> m_updateGuiTimer;
    std::unique_ptr<QGraphicsTextItem> m_txtHelp;
    std::unique_ptr<QGraphicsTextItem> m_txtGameOver;

    QBrush m_redBrush;
    QBrush m_yelBrush;
    QPen m_borderPen;
    QPen m_dashedPen;

    void init();
    void startTimer();
    void enable_columns();
    void disable_columns();
    void save_drop(int pos);
    void makelines();

    virtual void updatePositions(boardarray positions) override;
    virtual void updatePossibleDrops(std::vector<int> possibleDrops) override;
    virtual void writeToLog(QString item) override;
    virtual void gameOver(int winningPlayer) override;
    virtual void setWinningLine(std::pair<std::pair<int, int>, std::pair<int, int>> winningLine) override;

    //game start settings
    bool m_p1_is_ai;
    bool m_p2_is_ai;
    int m_p1_depth;
    int m_p2_depth;
    int m_p_start;

    //board variables during game
    boardarray m_positions;
    bool m_game_over;
    int m_winner;
    std::pair<std::pair<int, int>, std::pair<int, int>> m_winner_line;
    std::vector<int> m_possibleDrops;

private slots:
    void on_btn_drop_0_clicked();
    void on_btn_drop_1_clicked();
    void on_btn_drop_2_clicked();
    void on_btn_drop_3_clicked();
    void on_btn_drop_4_clicked();
    void on_btn_drop_5_clicked();
    void on_btn_drop_6_clicked();

    void on_btn_start_clicked();
    void on_btn_reset_clicked();

    void on_cmbb_difficulty_1_currentIndexChanged(int index);
    void on_cmbb_difficulty_2_currentIndexChanged(int index);
    void on_rdbtn_p1_ai_toggled(bool checked);
    void on_rdbtn_p1_human_toggled(bool checked);
    void on_rdbtn_p2_ai_toggled(bool checked);
    void on_rdbtn_p2_human_toggled(bool checked);
    void on_rdbtn_start_p1_toggled(bool checked);
    void on_rdbtn_start_p2_toggled(bool checked);
    void updateGUI();
};

#endif // FORM_H
