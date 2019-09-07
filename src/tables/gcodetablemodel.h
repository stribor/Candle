// This file is a part of "Candle" application.
// Copyright 2015-2016 Hayrullin Denis Ravilevich

#ifndef GCODETABLEMODEL_H
#define GCODETABLEMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <vector>

struct GCodeItem
{
    enum States { InQueue, Sent, Processed, Skipped };

    QString command;
    QString response;
    int line;
    QStringList args;
    States state;
};

class GCodeTableModel : public QAbstractTableModel
{
    Q_OBJECT
    // easy switch between containers to test which one is fastest, this can be platform/compiler/qt version dependant
    // on my machine std::vector was fastest, than QVector and than QList (it was neglect-able but still)
    using gcvec = std::vector<GCodeItem>;
//    using gcvec = QVector<GCodeItem>;
//    using gcvec = QList<GCodeItem>;
public:
    explicit GCodeTableModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    void clear();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    gcvec &data();

signals:

public slots:

private:
    gcvec m_data;
    QStringList m_headers;
};

#endif // GCODETABLEMODEL_H
