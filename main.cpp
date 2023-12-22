#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QMessageBox>
#include <QLineEdit>
#include <QKeyEvent>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>

class CustomToolbarWidget : public QWidget {
public:
    CustomToolbarWidget(QWidget *parent = nullptr)
            : QWidget(parent) {
        label = new QLabel("", this);
        input = new QLineEdit(this);
        input->setPlaceholderText("Wprowadź tekst");

        toggleButton = new QPushButton("Przełącz", this);

        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->addWidget(label);
        layout->addWidget(input);
        layout->addWidget(toggleButton);

        connect(toggleButton, &QPushButton::clicked, this, &CustomToolbarWidget::toggleLabelInput);
    }

    QString getText() const {
        return label->text();
    }

private slots:

    void toggleLabelInput() {
        if (input->isVisible()) {
            input->hide();
            label->setText(input->text());
            label->show();
        } else {
            label->hide();
            input->setText(label->text());
            input->show();
            input->setFocus();
        }
    }

private:
    QLabel *label;
    QLineEdit *input;
    QPushButton *toggleButton;
};

class AddPointDialog : public QDialog {
public:
    AddPointDialog(QWidget *parent = nullptr)
            : QDialog(parent) {
        setWindowTitle("Dodaj punkt");

        xLabel = new QLabel("Współrzędna X:", this);
        yLabel = new QLabel("Współrzędna Y:", this);

        xLineEdit = new QLineEdit(this);
        yLineEdit = new QLineEdit(this);

        addButton = new QPushButton("Dodaj punkt", this);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(xLabel);
        layout->addWidget(xLineEdit);
        layout->addWidget(yLabel);
        layout->addWidget(yLineEdit);
        layout->addWidget(addButton);

        connect(addButton, &QPushButton::clicked, this, &AddPointDialog::accept);
    }

    double getX() const {
        return xLineEdit->text().toDouble();
    }

    double getY() const {
        return yLineEdit->text().toDouble();
    }

private:
    QLabel *xLabel;
    QLabel *yLabel;
    QLineEdit *xLineEdit;
    QLineEdit *yLineEdit;
    QPushButton *addButton;
};

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget *parent = nullptr)
            : QMainWindow(parent) {
        toolbar = new QToolBar(this);
        addToolBar(Qt::TopToolBarArea, toolbar);

        // Dodajemy akcję do paska narzędziowego
        QAction *actionBottom = toolbar->addAction("_");
        connect(actionBottom, &QAction::triggered, this, [this]() {
            addToolBar(Qt::BottomToolBarArea, this->toolbar);
        });

        QAction *actionTop = toolbar->addAction("^");
        connect(actionTop, &QAction::triggered, this, [this]() {
            addToolBar(Qt::TopToolBarArea, this->toolbar);
        });

        QAction *actionLeft = toolbar->addAction("<");
        connect(actionLeft, &QAction::triggered, this, [this]() {
            addToolBar(Qt::LeftToolBarArea, this->toolbar);
        });

        QAction *actionRight = toolbar->addAction(">");
        connect(actionRight, &QAction::triggered, this, [this]() {
            addToolBar(Qt::RightToolBarArea, this->toolbar);
        });

        customToolbarWidget = new CustomToolbarWidget(this);
        toolbar->addWidget(customToolbarWidget);

        inputField = new QLineEdit(this);
        inputField->setReadOnly(true);
        toolbar->addWidget(inputField);

        inputField->setText("NWM");

        // Ustawiamy obsługę zdarzenia klawiatury
        installEventFilter(this);

        series = new QLineSeries();

        chart = new QChart();
        chart->addSeries(series);

        chart->setTitle("Prosty wykres liniowy");
        chart->createDefaultAxes();

        chartView = new QChartView(chart);
        setCentralWidget(chartView);

        QAction *actionAddPoint = toolbar->addAction("Dodaj Punkt");
        connect(actionAddPoint, &QAction::triggered, this, [this]() {
            AddPointDialog dialog;
            if (dialog.exec() == QDialog::Accepted) {
                double x = dialog.getX();
                double y = dialog.getY();

                series->append(x, y);
            }
        });

        QAction *actionExit = toolbar->addAction("Exit");
        connect(actionExit, &QAction::triggered, qApp, &QApplication::quit);
    }

    // Obsługa zdarzenia klawiatury
    void keyPressEvent(QKeyEvent *event) override {
        inputField->setText(event->text());
    }

private:
    QToolBar *toolbar;
    QLineEdit *inputField;
    QLineSeries *series;
    QChart *chart;
    QChartView *chartView;
    CustomToolbarWidget *customToolbarWidget;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
