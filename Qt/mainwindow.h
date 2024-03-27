#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../set.hpp"

#include <QMainWindow>
#include <QTableWidget>
#include <QtCharts/QChartView>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    class Dipinto {
    public:
      Dipinto() {}

      Dipinto(const QString& scuola, const QString& autore,
              const QString& soggetto, const QString& data,
              const QString& sala)
          : _scuola(scuola), _autore(autore), _soggetto(soggetto),
            _data(data), _sala(sala) {}

      QString GetScuola() const { return _scuola; }
      QString GetAutore() const { return _autore; }
      QString GetSoggetto() const { return _soggetto; }
      QString GetData() const { return _data; }
      QString GetSala() const { return _sala; }

    private:
      QString _scuola;
      QString _autore;
      QString _soggetto;
      QString _data;
      QString _sala;
    };

    class DipintoEquality {
    public:
        bool operator()(const Dipinto& a, const Dipinto& b) const {
            return a.GetScuola() == b.GetScuola() &&
                   a.GetAutore() == b.GetAutore() &&
                   a.GetSoggetto() == b.GetSoggetto() &&
                   a.GetData() == b.GetData() &&
                   a.GetSala() == b.GetSala();
        }
    };

    QStringList parseCsvLine(const QString &line);
    void loadCsvIntoSet(const QString &csvFilePath);
    void loadSetIntoTable();
    void setupTable();
    void createSchoolsPieChart();
    void createDatesBarChart();
    void updateCharts();

public slots:
    void filterTableContents(const QString& text);
    void onResetRircercaClicked();
    void onAggiungiDipintoClicked();
    void onRimuoviDipintoClicked();
    void onCambiaVisualizzazioneGraficoClicked();

private:
    Ui::MainWindow *ui;
    Set<Dipinto, DipintoEquality> setDipinti;

    QtCharts::QChartView* pieChartView;
    QtCharts::QChartView* barChartView;

    bool currentChartFirst;

    QVector<QColor> distinctColors = {
        QColor(52, 152, 219),   // #3498db
        QColor(46, 204, 113),   // #2ecc71
        QColor(241, 196, 15),   // #f1c40f
        QColor(231, 76, 60),    // #e74c3c
        QColor(155, 89, 182),   // #9b59b6
        QColor(52, 73, 94),     // #34495e
        QColor(22, 160, 133),   // #16a085
        QColor(39, 174, 96),    // #27ae60
        QColor(41, 128, 185),   // #2980b9
        QColor(44, 62, 80),     // #2c3e50
        QColor(243, 156, 18),   // #f39c12
    };

    //helper functions
    int findValidYear(const QString& text);
    bool containsValidYear(const QString& text);
};
#endif // MAINWINDOW_H
