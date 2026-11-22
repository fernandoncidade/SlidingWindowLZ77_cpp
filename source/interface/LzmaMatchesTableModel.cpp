#include "source/interface/LzmaMatchesTableModel.hpp"

LzmaMatchesTableModel::LzmaMatchesTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int LzmaMatchesTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return matches_.size();
}

int LzmaMatchesTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return ColumnCount;
}

QVariant LzmaMatchesTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= matches_.size()) {
        return QVariant();
    }

    const auto &m = matches_[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case ColIndex:
            return QString::number(index.row() + 1);
        case ColOffset:
            return QStringLiteral("0x%1 (%2)").arg(m.offset, 0, 16).arg(m.offset);
        case ColDistance:
            return QString::number(m.distance);
        case ColLength:
            return QString::number(m.length);
        case ColReference:
            return QStringLiteral("(%1, %2)").arg(m.distance).arg(m.length);
        default:
            break;
        }
    } else if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case ColIndex:
        case ColDistance:
        case ColLength:
            return static_cast<int>(Qt::AlignRight | Qt::AlignVCenter);
        case ColOffset:
        case ColReference:
            return static_cast<int>(Qt::AlignCenter);
        default:
            return static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }

    return QVariant();
}

QVariant LzmaMatchesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case ColIndex:
            return tr("Índice");
        case ColOffset:
            return tr("Offset de Saída");
        case ColDistance:
            return tr("Distância (d)");
        case ColLength:
            return tr("Comprimento (l)");
        case ColReference:
            return tr("Referência Curta (d, l)");
        default:
            break;
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

void LzmaMatchesTableModel::appendMatches(const QVector<LzmaMatchInfo> &newMatches)
{
    if (newMatches.isEmpty()) {
        return;
    }

    const int startRow = matches_.size();
    const int endRow = startRow + newMatches.size() - 1;

    beginInsertRows(QModelIndex(), startRow, endRow);
    matches_.append(newMatches);
    endInsertRows();
}

void LzmaMatchesTableModel::clear()
{
    if (matches_.isEmpty()) {
        return;
    }

    beginResetModel();
    matches_.clear();
    matches_.squeeze();
    endResetModel();
}

void LzmaMatchesTableModel::retranslate()
{
    emit headerDataChanged(Qt::Horizontal, 0, ColumnCount - 1);
}

quint64 LzmaMatchesTableModel::totalMatchesCount() const
{
    return static_cast<quint64>(matches_.size());
}
