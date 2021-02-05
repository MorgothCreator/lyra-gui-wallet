#include "responseparse.h"

/*responseParse::responseParse()
{

}

responseParse::~responseParse()
{

}*/

QString responseParse::print(QString text, QString item) {
    QStringList lst = text.split("\n");
    for(QString line : lst) {
        QStringList splItem = line.split(": ");
        if(splItem[0].contains(item)) {
            return splItem[1];
        }
    }
    return "";
}

QStringList responseParse::print(QString text, QStringList item) {
    QStringList lst = text.split("\n");
    QStringList retList;
    for(QString line : lst) {
        QStringList splItem = line.split(" ");
        for(QString lineItem : item) {
            if(splItem[0].contains(lineItem)) {
                if(splItem.count() == 2)
                    retList.append(splItem[1]);
                else
                    retList.append(splItem[1] + splItem[2]);
            }
        }
    }
    return retList;
}
