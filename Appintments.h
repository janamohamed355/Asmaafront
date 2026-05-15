// Appointments.h - Professional Version with Sidebar Spacing
#pragma once
#include "DbConnection.h"
namespace HospitalManagementSystem {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Drawing::Drawing2D;
    using namespace System::Collections::Generic;

    public ref class Appointments : public Form {
    private:
        Panel^ headerPanel; Panel^ contentPanel; Panel^ statsPanel; Panel^ mainContentPanel;
        DataGridView^ dgvAppointments; TextBox^ txtSearch;
        ComboBox^ cmbPatient; ComboBox^ cmbDoctor; DateTimePicker^ dtpDate;
        ComboBox^ cmbTime; ComboBox^ cmbStatus;
        Button^ btnAdd; Button^ btnUpdate; Button^ btnDelete; Button^ btnClear;
        Label^ lblDateTime; Label^ lblWelcome; PictureBox^ pbAvatar;
        Panel^ notificationPanel; Label^ lblNotificationCount;
        Timer^ timerDateTime; List<Dictionary<String^, String^>^>^ appointmentsList;
        int nextId; String^ userName;

    public:
        Appointments() {
            appointmentsList = gcnew List<Dictionary<String^, String^>^>();
            nextId = 5; userName = L"Admin";
            InitializeComponent(); LoadSampleData(); LoadStyles();
            RefreshGrid(); UpdateDateTime(); LoadStatsCards();
        }

    protected:
        ~Appointments() {
            if (timerDateTime != nullptr) delete timerDateTime;
        }

    private:
        String^ GetTimeOfDay() {
            int hour = DateTime::Now.Hour;
            if (hour < 12) return L"Morning";
            if (hour < 18) return L"Afternoon";
            return L"Evening";
        }

        void NotificationPanel_Paint(Object^ sender, PaintEventArgs^ e) {
            Control^ ctrl = safe_cast<Control^>(sender);
            GraphicsPath^ path = gcnew GraphicsPath();
            path->AddEllipse(0, 0, ctrl->Width, ctrl->Height);
            ctrl->Region = gcnew System::Drawing::Region(path);
            delete path;
        }

        void PbAvatar_Paint(Object^ sender, PaintEventArgs^ e) {
            PictureBox^ pic = safe_cast<PictureBox^>(sender);
            Graphics^ g = e->Graphics;
            g->SmoothingMode = SmoothingMode::AntiAlias;
            g->FillEllipse(gcnew SolidBrush(Color::FromArgb(155, 89, 182)), pic->ClientRectangle);
            String^ initial = userName->Length > 0 ? userName->Substring(0, 1)->ToUpper() : L"U";
            System::Drawing::Font^ f = gcnew System::Drawing::Font(L"Segoe UI", 14, FontStyle::Bold);
            SizeF textSize = g->MeasureString(initial, f);
            g->DrawString(initial, f, gcnew SolidBrush(Color::White),
                (pic->Width - textSize.Width) / 2, (pic->Height - textSize.Height) / 2);
            delete f;
        }

        void ApplyRoundedRegion(Control^ ctrl, int radius) {
            if (ctrl == nullptr || ctrl->Width == 0 || ctrl->Height == 0) return;
            GraphicsPath^ path = gcnew GraphicsPath();
            int d = radius * 2;
            path->AddArc(0, 0, d, d, 180, 90);
            path->AddArc(ctrl->Width - d, 0, d, d, 270, 90);
            path->AddArc(ctrl->Width - d, ctrl->Height - d, d, d, 0, 90);
            path->AddArc(0, ctrl->Height - d, d, d, 90, 90);
            path->CloseAllFigures();
            ctrl->Region = gcnew System::Drawing::Region(path);
            delete path;
        }

        void InitializeComponent() {
            this->Text = L"CareHub - Appointments Management";
            this->Size = System::Drawing::Size(1350, 800);
            this->StartPosition = FormStartPosition::CenterScreen;
            this->WindowState = FormWindowState::Maximized;
            this->BackColor = Color::FromArgb(240, 242, 245);
            this->Padding = System::Windows::Forms::Padding(0, 0, 0, 0);

            // ============ HEADER ============
            headerPanel = gcnew Panel();
            headerPanel->Dock = DockStyle::Top;
            headerPanel->Height = 70;
            headerPanel->BackColor = Color::White;
            headerPanel->Padding = System::Windows::Forms::Padding(20, 0, 20, 0);

            Label^ lblPageTitle = gcnew Label();
            lblPageTitle->Text = L"📅 Appointments Management";
            lblPageTitle->Font = gcnew System::Drawing::Font(L"Segoe UI", 16, FontStyle::Bold);
            lblPageTitle->ForeColor = Color::FromArgb(44, 62, 80);
            lblPageTitle->Location = Point(20, 18);
            lblPageTitle->Size = System::Drawing::Size(350, 35);

            lblWelcome = gcnew Label();
            lblWelcome->Text = L"Good " + GetTimeOfDay() + L", " + userName;
            lblWelcome->Font = gcnew System::Drawing::Font(L"Segoe UI", 12);
            lblWelcome->ForeColor = Color::FromArgb(100, 110, 140);
            lblWelcome->Location = Point(380, 25);
            lblWelcome->Size = System::Drawing::Size(250, 25);

            lblDateTime = gcnew Label();
            lblDateTime->Font = gcnew System::Drawing::Font(L"Segoe UI", 10);
            lblDateTime->ForeColor = Color::FromArgb(100, 110, 140);
            lblDateTime->Size = System::Drawing::Size(320, 25);
            lblDateTime->TextAlign = ContentAlignment::MiddleRight;
            lblDateTime->Anchor = AnchorStyles::Top | AnchorStyles::Right;

            notificationPanel = gcnew Panel();
            notificationPanel->Size = System::Drawing::Size(42, 42);
            notificationPanel->BackColor = Color::FromArgb(248, 249, 250);
            notificationPanel->Anchor = AnchorStyles::Top | AnchorStyles::Right;

            Label^ bellIcon = gcnew Label();
            bellIcon->Text = L"🔔";
            bellIcon->Font = gcnew System::Drawing::Font(L"Segoe UI", 16);
            bellIcon->Location = Point(10, 8);
            bellIcon->Size = System::Drawing::Size(28, 28);

            lblNotificationCount = gcnew Label();
            lblNotificationCount->Text = L"3";
            lblNotificationCount->Font = gcnew System::Drawing::Font(L"Segoe UI", 8, FontStyle::Bold);
            lblNotificationCount->ForeColor = Color::White;
            lblNotificationCount->BackColor = Color::FromArgb(231, 76, 60);
            lblNotificationCount->Size = System::Drawing::Size(18, 18);
            lblNotificationCount->Location = Point(26, 6);
            lblNotificationCount->TextAlign = ContentAlignment::MiddleCenter;

            notificationPanel->Controls->Add(bellIcon);
            notificationPanel->Controls->Add(lblNotificationCount);
            notificationPanel->Paint += gcnew PaintEventHandler(this, &Appointments::NotificationPanel_Paint);

            pbAvatar = gcnew PictureBox();
            pbAvatar->Size = System::Drawing::Size(48, 48);
            pbAvatar->BackColor = Color::FromArgb(155, 89, 182);
            pbAvatar->Anchor = AnchorStyles::Top | AnchorStyles::Right;
            pbAvatar->Paint += gcnew PaintEventHandler(this, &Appointments::PbAvatar_Paint);

            headerPanel->Controls->Add(lblPageTitle);
            headerPanel->Controls->Add(lblWelcome);
            headerPanel->Controls->Add(lblDateTime);
            headerPanel->Controls->Add(notificationPanel);
            headerPanel->Controls->Add(pbAvatar);

            // ============ CONTENT PANEL - مع مسافة من الشمال للـ Sidebar ============
            contentPanel = gcnew Panel();
            contentPanel->Dock = DockStyle::Fill;
            contentPanel->BackColor = Color::FromArgb(240, 242, 245);
            contentPanel->Padding = System::Windows::Forms::Padding(260, 15, 25, 20);
            contentPanel->AutoScroll = true;

            statsPanel = gcnew Panel();
            statsPanel->Dock = DockStyle::Top;
            statsPanel->Height = 145;
            statsPanel->BackColor = Color::Transparent;
            statsPanel->Padding = System::Windows::Forms::Padding(5, 5, 5, 5);

            mainContentPanel = gcnew Panel();
            mainContentPanel->Dock = DockStyle::Fill;
            mainContentPanel->BackColor = Color::White;
            mainContentPanel->Padding = System::Windows::Forms::Padding(20, 20, 20, 20);

            // Top Bar
            Panel^ topBar = gcnew Panel();
            topBar->Dock = DockStyle::Top;
            topBar->Height = 70;
            topBar->Padding = System::Windows::Forms::Padding(0, 5, 0, 10);

            Panel^ searchContainer = gcnew Panel();
            searchContainer->Location = Point(0, 8);
            searchContainer->Size = System::Drawing::Size(420, 45);
            searchContainer->BackColor = Color::FromArgb(248, 249, 250);
            searchContainer->BorderStyle = BorderStyle::FixedSingle;

            Label^ lblSearchIcon = gcnew Label();
            lblSearchIcon->Text = L"🔍";
            lblSearchIcon->Font = gcnew System::Drawing::Font(L"Segoe UI", 14);
            lblSearchIcon->Location = Point(12, 10);
            lblSearchIcon->Size = System::Drawing::Size(28, 28);

            txtSearch = gcnew TextBox();
            txtSearch->Location = Point(45, 8);
            txtSearch->Size = System::Drawing::Size(360, 32);
            txtSearch->BorderStyle = BorderStyle::None;
            txtSearch->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);
            txtSearch->Text = L"Search by patient or doctor...";
            txtSearch->ForeColor = Color::FromArgb(150, 150, 150);
            txtSearch->Enter += gcnew EventHandler(this, &Appointments::txtSearch_Enter);
            txtSearch->Leave += gcnew EventHandler(this, &Appointments::txtSearch_Leave);
            txtSearch->TextChanged += gcnew EventHandler(this, &Appointments::txtSearch_TextChanged);

            searchContainer->Controls->Add(lblSearchIcon);
            searchContainer->Controls->Add(txtSearch);

            Button^ btnAddNew = gcnew Button();
            btnAddNew->Text = L"+ New Appointment";
            btnAddNew->Location = Point(440, 8);
            btnAddNew->Size = System::Drawing::Size(170, 45);
            btnAddNew->BackColor = Color::FromArgb(155, 89, 182);
            btnAddNew->ForeColor = Color::White;
            btnAddNew->FlatStyle = FlatStyle::Flat;
            btnAddNew->Font = gcnew System::Drawing::Font(L"Segoe UI", 11, FontStyle::Bold);
            btnAddNew->Cursor = Cursors::Hand;
            btnAddNew->Click += gcnew EventHandler(this, &Appointments::ScrollToInput);

            topBar->Controls->Add(searchContainer);
            topBar->Controls->Add(btnAddNew);

            // DataGridView
            dgvAppointments = gcnew DataGridView();
            dgvAppointments->Dock = DockStyle::Fill;
            dgvAppointments->BackgroundColor = Color::White;
            dgvAppointments->BorderStyle = BorderStyle::None;
            dgvAppointments->RowHeadersVisible = false;
            dgvAppointments->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
            dgvAppointments->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
            dgvAppointments->AllowUserToAddRows = false;
            dgvAppointments->RowTemplate->Height = 38;
            dgvAppointments->CellClick += gcnew DataGridViewCellEventHandler(this, &Appointments::dgvAppointments_CellClick);

            // Bottom Input Panel
            Panel^ inputPanel = gcnew Panel();
            inputPanel->Dock = DockStyle::Bottom;
            inputPanel->Height = 210;
            inputPanel->BackColor = Color::FromArgb(248, 249, 250);
            inputPanel->BorderStyle = BorderStyle::FixedSingle;
            inputPanel->Padding = System::Windows::Forms::Padding(15, 10, 15, 10);

            Label^ lblFormTitle = gcnew Label();
            lblFormTitle->Text = L"📝 Appointment Details";
            lblFormTitle->Font = gcnew System::Drawing::Font(L"Segoe UI", 13, FontStyle::Bold);
            lblFormTitle->ForeColor = Color::FromArgb(44, 62, 80);
            lblFormTitle->Location = Point(5, 8);
            lblFormTitle->Size = System::Drawing::Size(250, 30);

            Label^ lblPatient = gcnew Label();
            lblPatient->Text = L"Patient:*";
            lblPatient->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblPatient->Location = Point(15, 55);
            lblPatient->Size = System::Drawing::Size(70, 25);

            cmbPatient = gcnew ComboBox();
            cmbPatient->Location = Point(90, 52);
            cmbPatient->Size = System::Drawing::Size(200, 35);
            cmbPatient->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);
            cmbPatient->DropDownStyle = ComboBoxStyle::DropDownList;
            cmbPatient->Items->Add(L"Ahmed Mohamed"); cmbPatient->Items->Add(L"Fatima Ali");
            cmbPatient->Items->Add(L"Omar Hassan"); cmbPatient->Items->Add(L"Sara Mahmoud");
            cmbPatient->SelectedIndex = 0;

            Label^ lblDoctor = gcnew Label();
            lblDoctor->Text = L"Doctor:*";
            lblDoctor->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblDoctor->Location = Point(310, 55);
            lblDoctor->Size = System::Drawing::Size(70, 25);

            cmbDoctor = gcnew ComboBox();
            cmbDoctor->Location = Point(385, 52);
            cmbDoctor->Size = System::Drawing::Size(200, 35);
            cmbDoctor->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);
            cmbDoctor->DropDownStyle = ComboBoxStyle::DropDownList;
            cmbDoctor->Items->Add(L"Dr. Mohamed Ali"); cmbDoctor->Items->Add(L"Dr. Sara Hassan");
            cmbDoctor->Items->Add(L"Dr. Ahmed Kamel"); cmbDoctor->Items->Add(L"Dr. Nadia Fathy");
            cmbDoctor->SelectedIndex = 0;

            Label^ lblDate = gcnew Label();
            lblDate->Text = L"Date:*";
            lblDate->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblDate->Location = Point(605, 55);
            lblDate->Size = System::Drawing::Size(60, 25);

            dtpDate = gcnew DateTimePicker();
            dtpDate->Location = Point(670, 52);
            dtpDate->Size = System::Drawing::Size(160, 35);
            dtpDate->Format = DateTimePickerFormat::Short;
            dtpDate->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);

            Label^ lblTime = gcnew Label();
            lblTime->Text = L"Time:*";
            lblTime->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblTime->Location = Point(15, 105);
            lblTime->Size = System::Drawing::Size(60, 25);

            cmbTime = gcnew ComboBox();
            cmbTime->Location = Point(80, 102);
            cmbTime->Size = System::Drawing::Size(140, 35);
            cmbTime->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);
            cmbTime->DropDownStyle = ComboBoxStyle::DropDownList;
            cmbTime->Items->Add(L"09:00 AM"); cmbTime->Items->Add(L"10:00 AM"); cmbTime->Items->Add(L"11:00 AM");
            cmbTime->Items->Add(L"12:00 PM"); cmbTime->Items->Add(L"02:00 PM"); cmbTime->Items->Add(L"03:00 PM");
            cmbTime->Items->Add(L"04:00 PM"); cmbTime->SelectedIndex = 0;

            Label^ lblStatus = gcnew Label();
            lblStatus->Text = L"Status:";
            lblStatus->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblStatus->Location = Point(240, 105);
            lblStatus->Size = System::Drawing::Size(60, 25);

            cmbStatus = gcnew ComboBox();
            cmbStatus->Location = Point(305, 102);
            cmbStatus->Size = System::Drawing::Size(150, 35);
            cmbStatus->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);
            cmbStatus->DropDownStyle = ComboBoxStyle::DropDownList;
            cmbStatus->Items->Add(L"Scheduled"); cmbStatus->Items->Add(L"Completed");
            cmbStatus->Items->Add(L"Cancelled"); cmbStatus->Items->Add(L"No Show");
            cmbStatus->SelectedIndex = 0;

            btnAdd = CreateButton(L"➕ Add", Color::FromArgb(46, 204, 113), 15, 158, 110, 42);
            btnUpdate = CreateButton(L"✏️ Update", Color::FromArgb(52, 152, 219), 140, 158, 110, 42);
            btnDelete = CreateButton(L"🗑️ Delete", Color::FromArgb(231, 76, 60), 265, 158, 110, 42);
            btnClear = CreateButton(L"🔄 Clear", Color::FromArgb(149, 165, 166), 390, 158, 110, 42);

            btnAdd->Click += gcnew EventHandler(this, &Appointments::btnAdd_Click);
            btnUpdate->Click += gcnew EventHandler(this, &Appointments::btnUpdate_Click);
            btnDelete->Click += gcnew EventHandler(this, &Appointments::btnDelete_Click);
            btnClear->Click += gcnew EventHandler(this, &Appointments::btnClear_Click);

            inputPanel->Controls->Add(lblFormTitle);
            inputPanel->Controls->Add(lblPatient); inputPanel->Controls->Add(cmbPatient);
            inputPanel->Controls->Add(lblDoctor); inputPanel->Controls->Add(cmbDoctor);
            inputPanel->Controls->Add(lblDate); inputPanel->Controls->Add(dtpDate);
            inputPanel->Controls->Add(lblTime); inputPanel->Controls->Add(cmbTime);
            inputPanel->Controls->Add(lblStatus); inputPanel->Controls->Add(cmbStatus);
            inputPanel->Controls->Add(btnAdd); inputPanel->Controls->Add(btnUpdate);
            inputPanel->Controls->Add(btnDelete); inputPanel->Controls->Add(btnClear);

            mainContentPanel->Controls->Add(topBar);
            mainContentPanel->Controls->Add(dgvAppointments);
            mainContentPanel->Controls->Add(inputPanel);

            contentPanel->Controls->Add(statsPanel);
            contentPanel->Controls->Add(mainContentPanel);

            this->Controls->Add(contentPanel);
            this->Controls->Add(headerPanel);

            timerDateTime = gcnew Timer();
            timerDateTime->Interval = 1000;
            timerDateTime->Tick += gcnew EventHandler(this, &Appointments::timerDateTime_Tick);
            timerDateTime->Start();

            this->Resize += gcnew EventHandler(this, &Appointments::OnResize);
        }

        Button^ CreateButton(String^ text, Color backColor, int x, int y, int width, int height) {
            Button^ btn = gcnew Button();
            btn->Text = text; btn->Location = Point(x, y); btn->Size = System::Drawing::Size(width, height);
            btn->BackColor = backColor; btn->ForeColor = Color::White; btn->FlatStyle = FlatStyle::Flat;
            btn->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold); btn->Cursor = Cursors::Hand;
            return btn;
        }

        void LoadStatsCards() {
            statsPanel->Controls->Clear();

            int scheduled = 0, completed = 0, cancelled = 0;
            for each(Dictionary<String^, String^> ^ apt in appointmentsList) {
                if (apt[L"Status"] == L"Scheduled") scheduled++;
                else if (apt[L"Status"] == L"Completed") completed++;
                else if (apt[L"Status"] == L"Cancelled") cancelled++;
            }

            // ✅ الحل: استخدام gcnew array بدلاً من initializer
            cli ::array<String^>^ titles = gcnew cli::array<String^>(4);
            titles[0] = L"Total Appointments";
            titles[1] = L"Scheduled";
            titles[2] = L"Completed";
            titles[3] = L"Cancelled";

            cli::array<String^>^ values = gcnew cli::array<String^>(4);
            values[0] = appointmentsList->Count.ToString();
            values[1] = scheduled.ToString();
            values[2] = completed.ToString();
            values[3] = cancelled.ToString();

            cli::array<String^>^ icons = gcnew cli::array<String^>(4);
            icons[0] = L"📅";
            icons[1] = L"⏳";
            icons[2] = L"✅";
            icons[3] = L"❌";

            cli::array<Color>^ colors = gcnew  cli::array<Color>(4);
            colors[0] = Color::FromArgb(155, 89, 182);
            colors[1] = Color::FromArgb(46, 204, 113);
            colors[2] = Color::FromArgb(52, 152, 219);
            colors[3] = Color::FromArgb(231, 76, 60);

            int cardWidth = 245;
            int startX = 5;

            for (int i = 0; i < 4; i++) {
                Panel^ card = gcnew Panel();
                card->Size = System::Drawing::Size(cardWidth, 115);
                card->Location = Point(startX + (i * (cardWidth + 12)), 8);
                card->BackColor = Color::White;

                Panel^ iconCircle = gcnew Panel();
                iconCircle->Size = System::Drawing::Size(48, 48);
                iconCircle->Location = Point(15, 15);
                iconCircle->BackColor = Color::FromArgb(234, 221, 245);

                Label^ iconLabel = gcnew Label();
                iconLabel->Text = icons[i];
                iconLabel->Font = gcnew System::Drawing::Font(L"Segoe UI", 22);
                iconLabel->Location = Point(9, 8);
                iconLabel->Size = System::Drawing::Size(35, 35);
                iconCircle->Controls->Add(iconLabel);

                Label^ title = gcnew Label();
                title->Text = titles[i];
                title->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
                title->ForeColor = Color::FromArgb(100, 110, 140);
                title->Location = Point(75, 20);
                title->Size = System::Drawing::Size(160, 25);

                Label^ value = gcnew Label();
                value->Text = values[i];
                value->Font = gcnew System::Drawing::Font(L"Segoe UI", 26, FontStyle::Bold);
                value->ForeColor = colors[i];
                value->Location = Point(75, 50);
                value->Size = System::Drawing::Size(160, 45);

                card->Controls->Add(iconCircle);
                card->Controls->Add(title);
                card->Controls->Add(value);

                ApplyRoundedRegion(card, 10);
                statsPanel->Controls->Add(card);
            }
        }

        void LoadSampleData() {
            AddAppointment(L"Ahmed Mohamed", L"Dr. Mohamed Ali", L"2024-01-15", L"10:00 AM", L"Scheduled");
            AddAppointment(L"Fatima Ali", L"Dr. Sara Hassan", L"2024-01-15", L"11:00 AM", L"Completed");
            AddAppointment(L"Omar Hassan", L"Dr. Ahmed Kamel", L"2024-01-16", L"09:00 AM", L"Scheduled");
            AddAppointment(L"Sara Mahmoud", L"Dr. Nadia Fathy", L"2024-01-16", L"02:00 PM", L"Scheduled");
        }

        void AddAppointment(String^ patient, String^ doctor, String^ date, String^ time, String^ status) {
            Dictionary<String^, String^>^ apt = gcnew Dictionary<String^, String^>();
            apt->Add(L"ID", (nextId++).ToString()); apt->Add(L"Patient", patient);
            apt->Add(L"Doctor", doctor); apt->Add(L"Date", date);
            apt->Add(L"Time", time); apt->Add(L"Status", status);
            appointmentsList->Add(apt);
        }

        void RefreshGrid() {
            dgvAppointments->Columns->Clear();
            dgvAppointments->Rows->Clear();

            // إعداد الأعمدة
            dgvAppointments->Columns->Add(L"ID", L"ID");
            dgvAppointments->Columns->Add(L"Patient", L"Patient Name");
            dgvAppointments->Columns->Add(L"Doctor", L"Doctor Name");
            dgvAppointments->Columns->Add(L"Date", L"Date");
            dgvAppointments->Columns->Add(L"Time", L"Time");
            dgvAppointments->Columns->Add(L"Status", L"Status");

            DbConnection db;
            sql::Connection* conn = db.getConnect();

            if (conn != nullptr) {
                try {
                    sql::Statement* stmt = conn->createStatement();
                    // استعلام لربط الجداول ببعضها لإظهار الأسماء
                    sql::ResultSet* res = stmt->executeQuery(
                        "SELECT a.id, p.name as p_name, d.name as d_name, a.app_date, a.app_time, a.status "
                        "FROM appointments a "
                        "JOIN patients p ON a.patient_id = p.id "
                        "JOIN doctors d ON a.doctor_id = d.id"
                    );

                    while (res->next()) {
                        dgvAppointments->Rows->Add(
                            res->getInt("id").ToString(),
                            gcnew String(res->getString("p_name").c_str()),
                            gcnew String(res->getString("d_name").c_str()),
                            gcnew String(res->getString("app_date").c_str()),
                            gcnew String(res->getString("app_time").c_str()),
                            gcnew String(res->getString("status").c_str())
                        );
                    }
                    delete res; delete stmt; delete conn;
                }
                catch (sql::SQLException& e) {
                    MessageBox::Show(L"Database Error: " + gcnew String(e.what()));
                }
            }
            LoadStatsCards();
        }

        void SearchAppointments(String^ searchTerm) {
            dgvAppointments->Rows->Clear();
            for each(Dictionary<String^, String^> ^ apt in appointmentsList) {
                if (apt[L"Patient"]->ToLower()->Contains(searchTerm->ToLower()) ||
                    apt[L"Doctor"]->ToLower()->Contains(searchTerm->ToLower())) {
                    dgvAppointments->Rows->Add(apt[L"ID"], apt[L"Patient"], apt[L"Doctor"],
                        apt[L"Date"], apt[L"Time"], apt[L"Status"]);
                }
            }
        }

        void LoadStyles() {
            dgvAppointments->EnableHeadersVisualStyles = false;
            dgvAppointments->ColumnHeadersDefaultCellStyle->BackColor = Color::FromArgb(155, 89, 182);
            dgvAppointments->ColumnHeadersDefaultCellStyle->ForeColor = Color::White;
            dgvAppointments->ColumnHeadersDefaultCellStyle->Font = gcnew System::Drawing::Font(L"Segoe UI", 11, FontStyle::Bold);
            dgvAppointments->AlternatingRowsDefaultCellStyle->BackColor = Color::FromArgb(248, 249, 250);
            ApplyRoundedRegion(mainContentPanel, 12);
        }

        void OnResize(Object^ sender, EventArgs^ e) {
            lblDateTime->Location = Point(this->ClientSize.Width - 340, 22);
            notificationPanel->Location = Point(this->ClientSize.Width - 170, 14);
            pbAvatar->Location = Point(this->ClientSize.Width - 100, 11);
        }

        void timerDateTime_Tick(Object^ sender, EventArgs^ e) { UpdateDateTime(); }
        void UpdateDateTime() { lblDateTime->Text = DateTime::Now.ToString(L"dddd, MMMM dd, yyyy  hh:mm:ss tt"); }
        void ScrollToInput(Object^ sender, EventArgs^ e) { cmbPatient->Focus(); }

        void txtSearch_Enter(Object^ sender, EventArgs^ e) {
            if (txtSearch->Text == L"Search by patient or doctor...") {
                txtSearch->Text = L""; txtSearch->ForeColor = Color::Black;
            }
        }

        void txtSearch_Leave(Object^ sender, EventArgs^ e) {
            if (txtSearch->Text->Length == 0) {
                txtSearch->Text = L"Search by patient or doctor...";
                txtSearch->ForeColor = Color::FromArgb(150, 150, 150);
            }
        }

        void txtSearch_TextChanged(Object^ sender, EventArgs^ e) {
            if (txtSearch->Text != L"Search by patient or doctor..." && txtSearch->Text->Length > 0)
                SearchAppointments(txtSearch->Text);
            else RefreshGrid();
        }

        void dgvAppointments_CellClick(Object^ sender, DataGridViewCellEventArgs^ e) {
            if (e->RowIndex >= 0) {
                DataGridViewRow^ row = dgvAppointments->Rows[e->RowIndex];
                cmbPatient->Text = row->Cells[1]->Value->ToString();
                cmbDoctor->Text = row->Cells[2]->Value->ToString();
                dtpDate->Value = DateTime::Parse(row->Cells[3]->Value->ToString());
                cmbTime->Text = row->Cells[4]->Value->ToString();
                cmbStatus->Text = row->Cells[5]->Value->ToString();
            }
        }

        void btnAdd_Click(Object^ sender, EventArgs^ e) {
            // التأكد أن المستخدم اختار مريض ودكتور
            if (cmbPatient->SelectedIndex == -1 || cmbDoctor->SelectedIndex == -1) {
                MessageBox::Show(L"Please select both Patient and Doctor", L"Validation Error", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            DbConnection db;
            sql::Connection* conn = db.getConnect();

            if (conn != nullptr) {
                try {
                    // تحويل البيانات من C++/CLI String إلى Native C++ String
                    using namespace System::Runtime::InteropServices;

                    // تحويل اسم المريض والدكتور المختارين
                    IntPtr pNamePtr = Marshal::StringToHGlobalAnsi(cmbPatient->SelectedItem->ToString());
                    IntPtr dNamePtr = Marshal::StringToHGlobalAnsi(cmbDoctor->SelectedItem->ToString());
                    IntPtr datePtr = Marshal::StringToHGlobalAnsi(dtpDate->Value.ToString("yyyy-MM-dd"));
                    IntPtr timePtr = Marshal::StringToHGlobalAnsi(cmbTime->SelectedItem->ToString());
                    IntPtr statusPtr = Marshal::StringToHGlobalAnsi(cmbStatus->SelectedItem->ToString());

                    try {
                        const char* patientName = (const char*)pNamePtr.ToPointer();
                        const char* doctorName = (const char*)dNamePtr.ToPointer();
                        const char* appDate = (const char*)datePtr.ToPointer();
                        const char* appTime = (const char*)timePtr.ToPointer();
                        const char* appStatus = (const char*)statusPtr.ToPointer();

                        // استعلام الإضافة باستخدام Sub-queries لجلب الـ IDs أوتوماتيكياً من أساميهم
                        sql::PreparedStatement* pstmt = conn->prepareStatement(
                            "INSERT INTO appointments (patient_id, doctor_id, app_date, app_time, status) "
                            "VALUES ("
                            "  (SELECT id FROM patients WHERE name = ? LIMIT 1), "
                            "  (SELECT id FROM doctors WHERE name = ? LIMIT 1), "
                            "  ?, ?, ?)"
                        );

                        pstmt->setString(1, patientName);
                        pstmt->setString(2, doctorName);
                        pstmt->setString(3, appDate);
                        pstmt->setString(4, appTime);
                        pstmt->setString(5, appStatus);

                        pstmt->executeUpdate();
                        delete pstmt;

                        // تحديث الجدول وتنظيف الخانات
                        RefreshGrid();
                        ClearFields();

                        MessageBox::Show(L"✓ Appointment added to Database successfully!", L"Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
                    }
                    finally {
                        // تنظيف الذاكرة آمناً
                        Marshal::FreeHGlobal(pNamePtr);
                        Marshal::FreeHGlobal(dNamePtr);
                        Marshal::FreeHGlobal(datePtr);
                        Marshal::FreeHGlobal(timePtr);
                        Marshal::FreeHGlobal(statusPtr);
                    }
                }
                catch (sql::SQLException& e) {
                    MessageBox::Show(L"Database Error: " + gcnew String(e.what()), L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                }
                catch (Exception^ e) {
                    MessageBox::Show(L"Error: " + e->Message, L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                }
            }
        }

        void btnUpdate_Click(Object^ sender, EventArgs^ e) {
            // Ensure a row is selected
            if (dgvAppointments->SelectedRows->Count == 0) {
                MessageBox::Show(L"Please select an appointment from the table to update", L"Selection Required", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            int appointmentId = Convert::ToInt32(dgvAppointments->SelectedRows[0]->Cells["ID"]->Value);

            DbConnection db;
            sql::Connection* conn = db.getConnect();

            if (conn != nullptr) {
                try {
                    using namespace System::Runtime::InteropServices;

                    // Convert new values from input fields
                    IntPtr pNamePtr = Marshal::StringToHGlobalAnsi(cmbPatient->Text);
                    IntPtr dNamePtr = Marshal::StringToHGlobalAnsi(cmbDoctor->Text);
                    IntPtr datePtr = Marshal::StringToHGlobalAnsi(dtpDate->Value.ToString("yyyy-MM-dd"));
                    IntPtr timePtr = Marshal::StringToHGlobalAnsi(cmbTime->Text);
                    IntPtr statusPtr = Marshal::StringToHGlobalAnsi(cmbStatus->Text);

                    try {
                        const char* pName = (const char*)pNamePtr.ToPointer();
                        const char* dName = (const char*)dNamePtr.ToPointer();
                        const char* aDate = (const char*)datePtr.ToPointer();
                        const char* aTime = (const char*)timePtr.ToPointer();
                        const char* aStatus = (const char*)statusPtr.ToPointer();

                        // Execute UPDATE query (using subqueries to resolve IDs from names)
                        sql::PreparedStatement* pstmt = conn->prepareStatement(
                            "UPDATE appointments SET "
                            "patient_id = (SELECT id FROM patients WHERE name = ? LIMIT 1), "
                            "doctor_id = (SELECT id FROM doctors WHERE name = ? LIMIT 1), "
                            "app_date = ?, app_time = ?, status = ? "
                            "WHERE id = ?"
                        );

                        pstmt->setString(1, pName);
                        pstmt->setString(2, dName);
                        pstmt->setString(3, aDate);
                        pstmt->setString(4, aTime);
                        pstmt->setString(5, aStatus);
                        pstmt->setInt(6, appointmentId);

                        pstmt->executeUpdate();
                        delete pstmt;

                        // Refresh display
                        RefreshGrid();
                        MessageBox::Show(L"✓ Appointment updated successfully!", L"Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
                    }
                    finally {
                        Marshal::FreeHGlobal(pNamePtr);
                        Marshal::FreeHGlobal(dNamePtr);
                        Marshal::FreeHGlobal(datePtr);
                        Marshal::FreeHGlobal(timePtr);
                        Marshal::FreeHGlobal(statusPtr);
                    }
                }
                catch (sql::SQLException& ex) {
                    MessageBox::Show(L"Update Error: " + gcnew String(ex.what()), L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                }
                catch (Exception^ ex) {
                    MessageBox::Show(L"Error: " + ex->Message, L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                }
            }
        }

        void btnDelete_Click(Object^ sender, EventArgs^ e) {
            // 1. التأكد أن المستخدم اختار صف (موعد) من الجدول
            if (dgvAppointments->SelectedRows->Count > 0) {
                // الحصول على الـ ID الخاص بالموعد المختار من أول عمود
                String^ appointmentID = dgvAppointments->SelectedRows[0]->Cells[0]->Value->ToString();

                // 2. رسالة تأكيد قبل الحذف (عشان اليوزر ميمسحش حاجة بالغلط)
                if (MessageBox::Show(L"Are you sure you want to cancel and delete Appointment ID: " + appointmentID + L"?",
                    L"Confirm Delete", MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {

                    DbConnection db;
                    sql::Connection* conn = db.getConnect();

                    if (conn != nullptr) {
                        try {
                            // 3. تنفيذ استعلام الحذف
                            sql::PreparedStatement* pstmt = conn->prepareStatement("DELETE FROM appointments WHERE id = ?");

                            // تحويل الـ ID من String لـ Integer
                            pstmt->setInt(1, System::Int32::Parse(appointmentID));

                            pstmt->executeUpdate();

                            delete pstmt;
                            delete conn;

                            // 4. تحديث الجدول وتنظيف الخانات
                            RefreshGrid();
                            ClearFields();

                            MessageBox::Show(L"✓ Appointment deleted successfully!", L"Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
                        }
                        catch (sql::SQLException& e) {
                            MessageBox::Show(L"Database Error: " + gcnew String(e.what()), L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                        }
                    }
                }
            }
            else {
                MessageBox::Show(L"Please select an appointment from the list to delete.", L"No Selection", MessageBoxButtons::OK, MessageBoxIcon::Warning);
            }
        }
        void btnClear_Click(Object^ sender, EventArgs^ e) { ClearFields(); }
        void ClearFields() { cmbPatient->SelectedIndex = 0; cmbDoctor->SelectedIndex = 0; dtpDate->Value = DateTime::Now; cmbTime->SelectedIndex = 0; cmbStatus->SelectedIndex = 0; }
    };
}
