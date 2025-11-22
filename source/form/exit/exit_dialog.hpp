#ifndef EXIT_DIALOG_HPP



#define EXIT_DIALOG_HPP



#include <QLabel>
#include <QDialog>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QCloseEvent>
#include <QRadioButton>
#include <QButtonGroup>
#include <QProgressBar>
#include <QStackedWidget>



struct ExitSaveOptions
{
    bool saveCode   = false;
    bool saveField  = false;
    bool savePlayer = false;
    bool saveFlags  = false;
    bool saveEnemies= false;
    enum ThemeChoice {PROTOTYPE, FOREST} theme = PROTOTYPE;
    QString outPath;
};



enum SaveMask : uint32_t{  SM_CODE    = 1 << 0 ,
                           SM_FIELD   = 1 << 1 ,
                           SM_PLAYER  = 1 << 2 ,
                           SM_FLAGS   = 1 << 3 ,
                           SM_ENEMIES = 1 << 4 ,
                           SM_THEME   = 1 << 5};



class ExitDialog : public QDialog
{
    Q_OBJECT
    public:
    explicit ExitDialog(QWidget *p=nullptr) : QDialog(p)
    {
        setMinimumSize(800, 500);
        setMaximumSize(1920, 1200);
        setWindowTitle("Draftsman");
        setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        auto *root = new QVBoxLayout(this);
        progress_ = new QProgressBar(this);
        progress_->setRange(0, 100);
        progress_->setTextVisible(false);
        progress_->setFixedHeight(20);
        progress_->setStyleSheet("QProgressBar::chunk {"
                                 "  background-color: #FB8C00;"
                                 "  border-radius: 5px;"
                                 "}");
        progress_->setVisible(false);
        root->addWidget(progress_);
        stack_ = new QStackedWidget(this);
        root->addWidget(stack_, 1);
        buildPage1();
        buildPage2();
        buildPage3();
        stack_->setCurrentIndex(0);
        updateProgressVisibility();
        connect(this, &QDialog::rejected, this, [this](){});
        connect(stack_, &QStackedWidget::currentChanged, this, [this](int){
            updateProgressVisibility();
        });
    }

    ExitSaveOptions options() const { return opts_; }

    protected:
    void closeEvent(QCloseEvent *ev) override
    {
        ev->accept();
    }

    private:
    QStackedWidget *stack_{}                ;
    QProgressBar   *progress_{}             ;
    QLabel         *p1Label_{}, *p2Label_{} , *p3Label_{} ;
    QPushButton    *p1Save_{} , *p1NoSave_{}, *p1Cancel_{};
    QCheckBox      *cbCode_{} , *cbField_{} , *cbPlayer_{}, *cbFlags_{}, *cbEnemies_{};
    QPushButton    *p2Back_{} , *p2Next_{}  ;
    QRadioButton   *rbProto_{}, *rbForest_{};
    QButtonGroup   *themeGroup_{}           ;
    QPushButton    *p3Back_{} , *p3Finish_{};
    ExitSaveOptions opts_;
    void buildPage1()
    {
        auto *p1 = new QWidget(this);
        auto *l = new QVBoxLayout(p1);
        p1Label_ = new QLabel("Уже уходите...?", p1);
        p1Label_->setAlignment(Qt::AlignCenter);
        l->addStretch();
        l->addWidget(p1Label_);
        l->addStretch();
        auto *btns = new QHBoxLayout();
        p1Save_   = new QPushButton("Сохранить", p1);
        p1NoSave_ = new QPushButton("НЕ сохранять", p1);
        p1Cancel_ = new QPushButton("Отмена", p1);
        btns->addWidget(p1Save_,   1);
        btns->addWidget(p1NoSave_, 1);
        btns->addWidget(p1Cancel_, 1);
        l->addLayout(btns);
        stack_->addWidget(p1);
        connect(p1Save_,   &QPushButton::clicked, this, [this]() { stack_->setCurrentIndex(1); progress_->setValue(50); });
        connect(p1NoSave_, &QPushButton::clicked, this, [this]() { accept(); /* закрыть редактор */ });
        connect(p1Cancel_, &QPushButton::clicked, this, [this]() { reject(); /* просто диалог */ });
    }

    void buildPage2()
    {
        auto *p2 = new QWidget(this);
        auto *l = new QVBoxLayout(p2);
        p2Label_ = new QLabel("Что следует сохранить?", p2);
        p2Label_->setAlignment(Qt::AlignCenter);
        l->addWidget(p2Label_);
        cbCode_    = new QCheckBox("Код", p2);
        cbField_   = new QCheckBox("Поле", p2);
        cbPlayer_  = new QCheckBox("Игрок", p2);
        cbFlags_   = new QCheckBox("Флаги", p2);
        cbEnemies_ = new QCheckBox("Враги", p2);
        auto addCentered = [&](QCheckBox *b) {
            auto *line = new QHBoxLayout();
            line->addStretch();
            line->addWidget(b);
            line->addStretch();
            l->addLayout(line);
        };

        addCentered(cbCode_);
        addCentered(cbField_);
        addCentered(cbPlayer_);
        addCentered(cbFlags_);
        addCentered(cbEnemies_);
        cbCode_->setEnabled(true);
        cbField_->setEnabled(true);
        setFieldDependentsEnabled(false);
        auto *btns = new QHBoxLayout();
        p2Back_ = new QPushButton("Назад", p2);
        p2Next_ = new QPushButton("Далее", p2);
        btns->addWidget(p2Back_, 1);
        btns->addStretch();
        btns->addWidget(p2Next_, 1);
        l->addStretch();
        l->addLayout(btns);
        stack_->addWidget(p2);
        connect(cbField_, &QCheckBox::toggled, this, [this](bool on){
            if(!on) {
                cbPlayer_->setChecked(false);
                cbFlags_->setChecked(false);
                cbEnemies_->setChecked(false);
            }

            setFieldDependentsEnabled(on);
            updateNextEnabled();
        });

        connect(cbCode_,    &QCheckBox::toggled, this, [this](bool){ updateNextEnabled(); });
        connect(cbPlayer_,  &QCheckBox::toggled, this, [this](bool){ updateNextEnabled(); });
        connect(cbFlags_,   &QCheckBox::toggled, this, [this](bool){ updateNextEnabled(); });
        connect(cbEnemies_, &QCheckBox::toggled, this, [this](bool){ updateNextEnabled(); });
        connect(p2Back_, &QPushButton::clicked, this, [this]() {stack_->setCurrentIndex(0);});
        connect(p2Next_, &QPushButton::clicked, this, [this]() {
            if(!p2Next_->isEnabled()) {
                return;
            }

            opts_.saveCode    = cbCode_->isChecked();
            opts_.saveField   = cbField_->isChecked();
            opts_.savePlayer  = cbPlayer_->isChecked();
            opts_.saveFlags   = cbFlags_->isChecked();
            opts_.saveEnemies = cbEnemies_->isChecked();
            stack_->setCurrentIndex(2);
            progress_->setValue(100);
        });

        updateNextEnabled();
    }

    void buildPage3()
    {
        auto *p3 = new QWidget(this);
        auto *l = new QVBoxLayout(p3);
        p3Label_ = new QLabel("Выберите тему:", p3);
        p3Label_->setAlignment(Qt::AlignCenter);
        l->addWidget(p3Label_);
        rbProto_  = new QRadioButton("Прототип", p3);
        rbForest_ = new QRadioButton("Лес", p3);
        rbProto_->setChecked(true);
        int32_t fixedW = 220;
        rbProto_->setFixedWidth(fixedW);
        rbForest_->setFixedWidth(fixedW);
        auto *radioBox = new QVBoxLayout();
        radioBox->setSpacing(12);
        auto *row1 = new QHBoxLayout(); row1->addStretch(); row1->addWidget(rbProto_ ); row1->addStretch();
        auto *row2 = new QHBoxLayout(); row2->addStretch(); row2->addWidget(rbForest_); row2->addStretch();
        radioBox->addLayout(row1);
        radioBox->addLayout(row2);
        l->addLayout(radioBox);
        themeGroup_ = new QButtonGroup(this);
        themeGroup_->addButton(rbProto_,  0);
        themeGroup_->addButton(rbForest_, 1);
        auto *btns = new QHBoxLayout();
        p3Back_   = new QPushButton("Назад", p3);
        p3Finish_ = new QPushButton("Завершить", p3);
        btns->addWidget(p3Back_, 1);
        btns->addStretch();
        btns->addWidget(p3Finish_, 1);
        l->addStretch();
        l->addLayout(btns);
        stack_->addWidget(p3);
        connect(p3Back_, &QPushButton::clicked,  this, [this]() {stack_->setCurrentIndex(1); progress_->setValue(50);});
        connect(p3Finish_, &QPushButton::clicked, this, [this]() {
            QString out = QFileDialog::getSaveFileName(this, "Сохранить файл", "level.dat", "DAT (*.dat)");
            if(out.isEmpty()) {
                return;
            }

            opts_.theme = rbForest_->isChecked() ? ExitSaveOptions::FOREST : ExitSaveOptions::PROTOTYPE;
            opts_.outPath = out;
            accept();
        });
    }

    void updateProgressVisibility()
    {
        int32_t idx = stack_->currentIndex();
        progress_->setVisible(idx >= 1);
    }

    void setFieldDependentsEnabled(bool on)
    {
        cbPlayer_->setEnabled(on);
        cbFlags_->setEnabled(on);
        cbEnemies_->setEnabled(on);
    }

    void updateNextEnabled()
    {
        bool any = cbCode_->isChecked  ()
                                       || cbField_->isChecked()
                                       || cbPlayer_->isChecked()
                                       || cbFlags_->isChecked()
                                       || cbEnemies_->isChecked();
        if(!cbField_->isChecked()) {
            any = cbCode_->isChecked() || cbField_->isChecked();
        }

        p2Next_->setEnabled(any);
    }
};



#endif  // EXIT_DIALOG_HPP
