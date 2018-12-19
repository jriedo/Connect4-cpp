#ifndef OBSERVER_H
#define OBSERVER_H

#include <QString>


class Observer
{
    using boardarray = std::array<std::array<int, 6>, 7>;

public:
    virtual ~Observer(){}
    virtual void updatePositions(boardarray positions) = 0;
    virtual void updatePossibleDrops(std::vector<int> possibleDrops) = 0;
    virtual void writeToLog(QString item) = 0;
    virtual void gameOver(int winningPlayer) = 0;
    virtual void setWinningLine(std::pair<std::pair<int, int>, std::pair<int, int>> winningLine) = 0;
};

#endif // OBSERVER_H
