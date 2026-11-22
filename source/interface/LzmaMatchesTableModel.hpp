#pragma once

#include <QAbstractTableModel>
#include <QVector>
#include "source/config/LzmaExtractor.hpp"

class LzmaMatchesTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        ColIndex = 0,
        ColOffset,
        ColDistance,
        ColLength,
        ColReference,
        ColumnCount
    };

    explicit LzmaMatchesTableModel(QObject *parent = nullptr);
    ~LzmaMatchesTableModel() override = default;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void appendMatches(const QVector<LzmaMatchInfo> &newMatches);
    void clear();
    void retranslate();

    quint64 totalMatchesCount() const;

private:
    QVector<LzmaMatchInfo> matches_;
};
