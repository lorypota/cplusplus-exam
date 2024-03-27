#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../set.hpp"

#include <QFile>
#include <QTextStream>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>
#include <QObject>
#include <QMessageBox>
#include <QMap>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QAbstractBarSeries>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loadCsvIntoSet(":/dipinti_uffizi.csv");
    loadSetIntoTable();

    // Connect button signals to the appropriate slot functions
    connect(ui->lineEdit_ricerca, SIGNAL(textChanged(QString)), this, SLOT(filterTableContents(QString)));
    connect(ui->pushButton_reset_ricerca, SIGNAL(clicked()), this, SLOT(onResetRircercaClicked()));
    connect(ui->pushButton_aggiungi, SIGNAL(clicked()), this, SLOT(onAggiungiDipintoClicked()));
    connect(ui->pushButton_rimuovi, SIGNAL(clicked()), this, SLOT(onRimuoviDipintoClicked()));
    connect(ui->pushButtton_cambia_grafico, SIGNAL(clicked()), this, SLOT(onCambiaVisualizzazioneGraficoClicked()));

    setupTable();

    createSchoolsPieChart();
    createDatesBarChart();

    // Add the chart views to a stacked widget
    ui->stackedWidget->addWidget(pieChartView);
    ui->stackedWidget->addWidget(barChartView);

    // Start by showing the pie chart
    ui->stackedWidget->setCurrentWidget(pieChartView);
    currentChartFirst = true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::findValidYear(const QString& text) {
    // This function finds the year inside of a string by finding the first
    // 3/4 consecutive digits in the range between 100 and 2024

    QString numberBuffer;
    for (int i = 0; i <= text.length(); ++i) {
        if (i == text.length() || !text[i].isDigit()) {
            if (numberBuffer.length() == 3 || numberBuffer.length() == 4) {
                int year = numberBuffer.toInt();
                if (year >= 100 && year <= 2024) {
                    return year;
                }
            }
            numberBuffer.clear();
        } else {
            numberBuffer.append(text[i]);
        }
    }
    return 0; // No valid year found, only needed range is between 100 and 2024
}

bool MainWindow::containsValidYear(const QString& text) {
    if(findValidYear(text) == 0) {
        return false;
    }
    return true;
}

QStringList MainWindow::parseCsvLine(const QString &line) {
    enum class State { Normal, Quote } state = State::Normal;
    QStringList fields;
    QString value;

    for (int i = 0; i < line.size(); ++i) {
        QChar current = line[i];

        if (state == State::Normal) {
            if (current == ',') {
                fields << value.trimmed();
                value.clear();
            } else if (current == '"') {
                state = State::Quote;
            } else {
                value += current;
            }
        } else if (state == State::Quote) {
            if (current == '"') {
                if (i+1 < line.size() && line[i+1] == '"') {
                    value += '"';
                    ++i;
                } else {
                    state = State::Normal;
                }
            } else {
                value += current;
            }
        }
    }
    fields << value.trimmed();

    return fields;
}

void MainWindow::loadCsvIntoSet( const QString& csvFilePath) {
    QFile file(csvFilePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return;
    }

    QTextStream in(&file);

    QString headerLine = in.readLine();
    QStringList headerLabels = headerLine.split(",", QString::SkipEmptyParts);

    ui->tableWidget_dipinti->setColumnCount(headerLabels.size());
    ui->tableWidget_dipinti->setHorizontalHeaderLabels(headerLabels);

    int row = 0;

    while (!in.atEnd()) {
        QString fileLine = in.readLine();
        QStringList lineToken = parseCsvLine(fileLine);

        Dipinto dipinto(lineToken[0], lineToken[1], lineToken[2], lineToken[3], lineToken[4]);

        setDipinti.add(dipinto);
        ++row;
    }

    file.close();
}

void MainWindow::setupTable(){
    // Set header width to fill available space
    QHeaderView* headerView = ui->tableWidget_dipinti->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    // Make rows selectable (for removing items)
    ui->tableWidget_dipinti->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void MainWindow::loadSetIntoTable() {
    // Set up the row count based on the set size
    ui->tableWidget_dipinti->setRowCount(setDipinti.getNumElements());

    int row = 0;
    for (const auto& dipinto : setDipinti) {
        // Create QTableWidgetItem for each field in the Dipinto object
        ui->tableWidget_dipinti->setItem(row, 0, new QTableWidgetItem(dipinto.GetScuola()));
        ui->tableWidget_dipinti->setItem(row, 1, new QTableWidgetItem(dipinto.GetAutore()));
        ui->tableWidget_dipinti->setItem(row, 2, new QTableWidgetItem(dipinto.GetSoggetto()));
        ui->tableWidget_dipinti->setItem(row, 3, new QTableWidgetItem(dipinto.GetData()));
        ui->tableWidget_dipinti->setItem(row, 4, new QTableWidgetItem(dipinto.GetSala()));

        // Set the items to be non-editable
        for (int col = 0; col < 5; ++col) {
            QTableWidgetItem* item = ui->tableWidget_dipinti->item(row, col);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
        ++row;
    }
}

void MainWindow::createSchoolsPieChart() {
    QtCharts::QPieSeries* series = new QtCharts::QPieSeries();

    // Count the number of paintings per school
    QMap<QString, int> schoolCounts;
    for (int i = 0; i < ui->tableWidget_dipinti->rowCount(); ++i) {
        QString school = ui->tableWidget_dipinti->item(i, 0)->text(); // 0 = Scuola
        schoolCounts[school]++;
    }

    // Sort the schoools by count
    QVector<QPair<int, QString>> sortedSchools;
    for (auto it = schoolCounts.constBegin(); it != schoolCounts.constEnd(); ++it) {
        sortedSchools.append(qMakePair(it.value(), it.key()));
    }
    std::sort(sortedSchools.begin(), sortedSchools.end(), std::greater<QPair<int, QString>>());

    // Determine how many distinct slices to have based on the number of available colors
    int distinctSliceCount = qMin(distinctColors.size(), sortedSchools.size());

    // Determine number of total paintings
    int totalPaintings = setDipinti.getNumElements();

    // Counter for number of paintings in "other"
    int otherCount = 0;

    // Add slices for schools with the highest painting counts.
    // Schools beyond the number of distinct colors available get accumulated into 'Other'
    for (int i = 0; i < sortedSchools.size(); ++i) {
        int paintingCount = sortedSchools[i].first;
        const QString& schoolName = sortedSchools[i].second;

        // Calculate percentage of each slice
        double percentage = 100.0 * paintingCount / totalPaintings;

        // Set when to group inside of other and when not to (if percentage is < 2%, group into other to avoid a crowded graph)
        if (i < distinctSliceCount && percentage > 2) {
            QtCharts::QPieSlice* slice = series->append(schoolName, paintingCount);
            slice->setColor(distinctColors[i]);
            slice->setLabelVisible(true);
            slice->setLabel(QString("%1: %2%").arg(schoolName).arg(percentage, 0, 'f', 1));
        } else {
            otherCount += paintingCount;
        }
    }

    // Add an "Other" slice if necessary.
    if (otherCount > 0) {
        double percentage = 100.0 * otherCount / totalPaintings;

        QtCharts::QPieSlice* otherSlice = series->append("Altre", otherCount);
        otherSlice->setColor(QColor(149, 165, 166)); // Distinct color for "Other"
        otherSlice->setLabelVisible(true);
        otherSlice->setLabel(QString("Altre: %1%").arg(percentage, 0, 'f', 1));
    }

    // Create the chart and set the series
    QtCharts::QChart* chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle("Percentuale di dipinti per Scuola");

    // Customization
    chart->legend()->setVisible(false);

    // Creating the chart view and adding it to the layout
    pieChartView = new QtCharts::QChartView(chart);
    pieChartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::createDatesBarChart() {
    // Initialize min and max years to create barChart
    int minYear = 2024;
    int maxYear = 100;

    // Find min and max years inside table
    for (int i = 0; i < ui->tableWidget_dipinti->rowCount(); ++i) {
        QString yearStr = ui->tableWidget_dipinti->item(i, 3)->text();  // year is in the 4th column
        int year = findValidYear(yearStr);

        if (year < minYear) {
            minYear = year;
        }
        if (year > maxYear) {
            maxYear = year;
        }
    }

    // Determine the interval based on the number of colors and the years span
    int groupingInterval = 50; // Start with 50 years
    int numberOfGroups = (maxYear - minYear) / groupingInterval + 1;

    while (numberOfGroups > distinctColors.size()) {
        groupingInterval = groupingInterval * 1.5; // Increase the interval by half of its size
        numberOfGroups = (maxYear - minYear) / groupingInterval + 1;
    }

    QMap<QString, int> paintingsPerInterval;

    for (int i = 0; i < ui->tableWidget_dipinti->rowCount(); ++i) {

        // Extract the year from the "Data" column (4th column)
        QString yearStr = ui->tableWidget_dipinti->item(i, 3)->text();

        // Extract the year from the year string
        int year = findValidYear(yearStr);

        if(year == 0) {
            qDebug() << "Inserted year is not valid";
        } else {
            int intervalIndex = (year - minYear) / groupingInterval;

            // Calculate the start and end years for the interval
            int intervalStart = minYear + intervalIndex * groupingInterval;
            int intervalEnd = intervalStart + groupingInterval - 1;

            QString groupStr = QString::number(intervalStart) + "-" + QString::number(intervalEnd);
            paintingsPerInterval[groupStr]++;
        }
    }

    // Create the chart
    QtCharts::QChart* chart = new QtCharts::QChart();
    chart->setTitle("Numero di dipinti raggruppati ogni " + QString::number(groupingInterval) + " anni");

    // Create X axis with groups as categories
    QtCharts::QBarCategoryAxis* axisX = new QtCharts::QBarCategoryAxis();
    axisX->append(paintingsPerInterval.keys());

    // Calculate max height of axis Y (needed to handle cases when the bar is too short)
    const auto groupValues = paintingsPerInterval.values();
    double maxAxisValue = *std::max_element(groupValues.begin(), groupValues.end());

    // Iterate over each group to create a bar set and a series
    QList<QString> groupKeys = paintingsPerInterval.keys();

    for (int i = 0; i < groupKeys.size(); ++i) {
        QtCharts::QBarSeries* series = new QtCharts::QBarSeries();

        const QString& group = groupKeys.at(i);
        QtCharts::QBarSet* bar = new QtCharts::QBarSet(group);

        // Assign the value to the bar set
        *bar << paintingsPerInterval.value(group);

        // Assign a color to the bar set from the distinctColors list
        if(i >= distinctColors.size()) {
            qDebug() << "Error more groupings than colors";
        } else {
            QColor color = distinctColors.at(i);
            bar->setColor(color);
        }

        // handle cases when the bar is too short to display the value inside of it
        double heightRatio = paintingsPerInterval.value(group) / maxAxisValue;
        if (heightRatio < 0.1) {
            series->setLabelsPosition(QtCharts::QAbstractBarSeries::LabelsPosition::LabelsOutsideEnd);
            series->setLabelsFormat("<span style='color: black;'>@value</span>");
        }

        // Add the bar set to the series
        series->append(bar);
        series->setLabelsVisible(true);
        chart->addSeries(series);
    }

    // Build the chart
    chart->createDefaultAxes();
    chart->setAxisX(axisX); // Due to the old version of Qt of this project I need to use this deprecated version

    chart->legend()->setVisible(false);

    // Create the chart view
    barChartView = new QtCharts::QChartView(chart);
    barChartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::filterTableContents(const QString& text) {
    // Hide rows that match text
    for (int i = 0; i < ui->tableWidget_dipinti->rowCount(); ++i) {
        bool match = ui->tableWidget_dipinti->item(i, 2)->text().contains(text, Qt::CaseInsensitive);
        ui->tableWidget_dipinti->setRowHidden(i, !match);
    }
}

void MainWindow::onResetRircercaClicked() {
    // clear line edit
    ui->lineEdit_ricerca->clear();

    // show all rows
    for (int i = 0; i < ui->tableWidget_dipinti->rowCount(); ++i) {
        ui->tableWidget_dipinti->setRowHidden(i, false);
    }
}

void MainWindow::onAggiungiDipintoClicked() {
    // Retrieve input data
    QString scuola = ui->lineEdit_scuola->text().trimmed();
    QString autore = ui->lineEdit_autore->text().trimmed();
    QString soggetto = ui->lineEdit_soggetto->text().trimmed();
    QString data = ui->lineEdit_data->text().trimmed();
    QString sala = ui->lineEdit_sala->text().trimmed();

    // Check that all fields are not empty
    if (scuola.isEmpty() || autore.isEmpty() || soggetto.isEmpty() || data.isEmpty() || sala.isEmpty()) {
        QMessageBox::warning(this, tr("Dati incompleti"), tr("Per favore riempire tutti i campi."));
        return;
    }

    // Check that the 'Data' field contains a year with 3 or 4 digits, in between the year 100 and 2024
    if (!containsValidYear(data)) {
        QMessageBox::warning(this, tr("Data invalida"), tr("La data deve contenere un anno di 3 o 4 cifre, compreso tra 100 e 2024."));
        return;
    }

    Dipinto newDipinto(scuola, autore, soggetto, data, sala);

    // If add is successful, add new entry to table and clear fields
    if (setDipinti.add(newDipinto)) {
        int newRow = ui->tableWidget_dipinti->rowCount();
        ui->tableWidget_dipinti->insertRow(newRow);
        ui->tableWidget_dipinti->setItem(newRow, 0, new QTableWidgetItem(scuola));
        ui->tableWidget_dipinti->setItem(newRow, 1, new QTableWidgetItem(autore));
        ui->tableWidget_dipinti->setItem(newRow, 2, new QTableWidgetItem(soggetto));
        ui->tableWidget_dipinti->setItem(newRow, 3, new QTableWidgetItem(data));
        ui->tableWidget_dipinti->setItem(newRow, 4, new QTableWidgetItem(sala));

        ui->lineEdit_scuola->clear();
        ui->lineEdit_autore->clear();
        ui->lineEdit_soggetto->clear();
        ui->lineEdit_data->clear();
        ui->lineEdit_sala->clear();
    } else {
        QMessageBox::warning(this, tr("Dati ripetuti"), tr("I dati inseriti sono già stati salvati."));
    }

    updateCharts();
}

void MainWindow::onRimuoviDipintoClicked() {
    // Get the list of selected rows
    QList<QTableWidgetItem*> selectedItems = ui->tableWidget_dipinti->selectedItems();
    QList<int> rowsToRemove;

    // Return if nothing is selected
    if(selectedItems.size() == 0) {
        return;
    }

    // Collect all selected rows
    foreach(QTableWidgetItem* item, selectedItems) {
        int currentRow = item->row();
        if (!rowsToRemove.contains(currentRow)) {
            rowsToRemove.append(currentRow);
        }
    }

    // Sort the list of rows in descending order
    std::sort(rowsToRemove.begin(), rowsToRemove.end(), std::greater<int>());

    // Iterate over the rows in reverse order to remove the items
    foreach(int row, rowsToRemove) {
        // Create a Dipinto object with the data from the row
        QString scuola = ui->tableWidget_dipinti->item(row, 0)->text();
        QString autore = ui->tableWidget_dipinti->item(row, 1)->text();
        QString soggetto = ui->tableWidget_dipinti->item(row, 2)->text();
        QString data = ui->tableWidget_dipinti->item(row, 3)->text();
        QString sala = ui->tableWidget_dipinti->item(row, 4)->text();
        Dipinto dipintoToRemove(scuola, autore, soggetto, data, sala);

        // Remove the Dipinto from the Set
        if(setDipinti.remove(dipintoToRemove)) {
            // Remove the row from the QTableWidget
            ui->tableWidget_dipinti->removeRow(row);
        }
    }

    updateCharts();
}

void MainWindow::onCambiaVisualizzazioneGraficoClicked() {
    // Select and show chart that is not currently viewed
    if (ui->stackedWidget->currentWidget() == pieChartView) {
        ui->stackedWidget->setCurrentWidget(barChartView);
        currentChartFirst = false;
    } else {
        ui->stackedWidget->setCurrentWidget(pieChartView);
        currentChartFirst = true;
    }
}

void MainWindow::updateCharts() {
    // Remove and delete chart widgets
    while (ui->stackedWidget->count() > 0) {
        QWidget* widget = ui->stackedWidget->widget(0);
        ui->stackedWidget->removeWidget(widget);
        delete widget;
    }

    // Create widgets from scratch with the updated data
    createSchoolsPieChart();
    createDatesBarChart();

    // Add widgets to the stackedWidget
    ui->stackedWidget->addWidget(pieChartView);
    ui->stackedWidget->addWidget(barChartView);

    // Show currently selected widget (chart)
    if(currentChartFirst) {
        ui->stackedWidget->setCurrentWidget(pieChartView);
    } else {
        ui->stackedWidget->setCurrentWidget(barChartView);
    }
}
