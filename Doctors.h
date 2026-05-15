// Doctors.h - Professional Version with Sidebar Spacing
#pragma once
#include "DbConnection.h"
#include <iostream>
using namespace std;


void showDoctors() {
    DbConnection db;
    sql::Connection* conn = db.connect(); // فتحنا الباب

    if (conn != nullptr) {
        sql::Statement* stmt = conn->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT * FROM doctors");

        while (res->next()) {
            cout << "Dr. Name: " << res->getString("name") << endl;
        }

        delete res;
        delete stmt;
        db.close(); // قفلنا الباب عشان نحافظ على موارد الجهاز
    }
}
namespace HospitalManagementSystem {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Drawing::Drawing2D;
    using namespace System::Collections::Generic;

    public ref class Doctors : public Form {
    private:
        Panel^ headerPanel; Panel^ contentPanel; Panel^ statsPanel; Panel^ mainContentPanel;
        DataGridView^ dgvDoctors; TextBox^ txtSearch; TextBox^ txtName; TextBox^ txtSpecialization;
        TextBox^ txtPhone; TextBox^ txtEmail; Button^ btnAdd; Button^ btnUpdate; Button^ btnDelete; Button^ btnClear;
        Label^ lblDateTime; Label^ lblWelcome; PictureBox^ pbAvatar;
        Panel^ notificationPanel; Label^ lblNotificationCount;
        Timer^ timerDateTime; List<Dictionary<String^, String^>^>^ doctorsList;
        int nextId; String^ userName;

    public:
        Doctors() {
            doctorsList = gcnew List<Dictionary<String^, String^>^>();
            nextId = 5; userName = L"Admin";
            InitializeComponent(); LoadSampleData(); LoadStyles();
            RefreshGrid(); UpdateDateTime(); LoadStatsCards();
        }

    protected:
        ~Doctors() {
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
            g->FillEllipse(gcnew SolidBrush(Color::FromArgb(46, 204, 113)), pic->ClientRectangle);
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
            this->Text = L"CareHub - Doctors Management";
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
            lblPageTitle->Text = L"👨‍⚕️ Doctors Management";
            lblPageTitle->Font = gcnew System::Drawing::Font(L"Segoe UI", 16, FontStyle::Bold);
            lblPageTitle->ForeColor = Color::FromArgb(44, 62, 80);
            lblPageTitle->Location = Point(20, 18);
            lblPageTitle->Size = System::Drawing::Size(300, 35);

            lblWelcome = gcnew Label();
            lblWelcome->Text = L"Good " + GetTimeOfDay() + L", " + userName;
            lblWelcome->Font = gcnew System::Drawing::Font(L"Segoe UI", 12);
            lblWelcome->ForeColor = Color::FromArgb(100, 110, 140);
            lblWelcome->Location = Point(340, 25);
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
            notificationPanel->Paint += gcnew PaintEventHandler(this, &Doctors::NotificationPanel_Paint);

            pbAvatar = gcnew PictureBox();
            pbAvatar->Size = System::Drawing::Size(48, 48);
            pbAvatar->BackColor = Color::FromArgb(46, 204, 113);
            pbAvatar->Anchor = AnchorStyles::Top | AnchorStyles::Right;
            pbAvatar->Paint += gcnew PaintEventHandler(this, &Doctors::PbAvatar_Paint);

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

            // Stats Panel
            statsPanel = gcnew Panel();
            statsPanel->Dock = DockStyle::Top;
            statsPanel->Height = 145;
            statsPanel->BackColor = Color::Transparent;
            statsPanel->Padding = System::Windows::Forms::Padding(5, 5, 5, 5);

            // Main White Container
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
            txtSearch->Text = L"Search by name or specialization...";
            txtSearch->ForeColor = Color::FromArgb(150, 150, 150);
            txtSearch->Enter += gcnew EventHandler(this, &Doctors::txtSearch_Enter);
            txtSearch->Leave += gcnew EventHandler(this, &Doctors::txtSearch_Leave);
            txtSearch->TextChanged += gcnew EventHandler(this, &Doctors::txtSearch_TextChanged);

            searchContainer->Controls->Add(lblSearchIcon);
            searchContainer->Controls->Add(txtSearch);

            Button^ btnAddNew = gcnew Button();
            btnAddNew->Text = L"+ New Doctor";
            btnAddNew->Location = Point(440, 8);
            btnAddNew->Size = System::Drawing::Size(170, 45);
            btnAddNew->BackColor = Color::FromArgb(46, 204, 113);
            btnAddNew->ForeColor = Color::White;
            btnAddNew->FlatStyle = FlatStyle::Flat;
            btnAddNew->Font = gcnew System::Drawing::Font(L"Segoe UI", 11, FontStyle::Bold);
            btnAddNew->Cursor = Cursors::Hand;
            btnAddNew->Click += gcnew EventHandler(this, &Doctors::ScrollToInput);

            topBar->Controls->Add(searchContainer);
            topBar->Controls->Add(btnAddNew);

            // DataGridView
            dgvDoctors = gcnew DataGridView();
            dgvDoctors->Dock = DockStyle::Fill;
            dgvDoctors->BackgroundColor = Color::White;
            dgvDoctors->BorderStyle = BorderStyle::None;
            dgvDoctors->RowHeadersVisible = false;
            dgvDoctors->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
            dgvDoctors->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
            dgvDoctors->AllowUserToAddRows = false;
            dgvDoctors->RowTemplate->Height = 38;
            dgvDoctors->CellClick += gcnew DataGridViewCellEventHandler(this, &Doctors::dgvDoctors_CellClick);

            // Bottom Input Panel
            Panel^ inputPanel = gcnew Panel();
            inputPanel->Dock = DockStyle::Bottom;
            inputPanel->Height = 210;
            inputPanel->BackColor = Color::FromArgb(248, 249, 250);
            inputPanel->BorderStyle = BorderStyle::FixedSingle;
            inputPanel->Padding = System::Windows::Forms::Padding(15, 10, 15, 10);

            Label^ lblFormTitle = gcnew Label();
            lblFormTitle->Text = L"📝 Doctor Information Form";
            lblFormTitle->Font = gcnew System::Drawing::Font(L"Segoe UI", 13, FontStyle::Bold);
            lblFormTitle->ForeColor = Color::FromArgb(44, 62, 80);
            lblFormTitle->Location = Point(5, 8);
            lblFormTitle->Size = System::Drawing::Size(250, 30);

            Label^ lblName = gcnew Label();
            lblName->Text = L"Doctor Name:*";
            lblName->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblName->Location = Point(15, 55);
            lblName->Size = System::Drawing::Size(100, 25);

            txtName = gcnew TextBox();
            txtName->Location = Point(120, 52);
            txtName->Size = System::Drawing::Size(220, 35);
            txtName->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);

            Label^ lblSpecialization = gcnew Label();
            lblSpecialization->Text = L"Specialization:*";
            lblSpecialization->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblSpecialization->Location = Point(360, 55);
            lblSpecialization->Size = System::Drawing::Size(100, 25);

            txtSpecialization = gcnew TextBox();
            txtSpecialization->Location = Point(465, 52);
            txtSpecialization->Size = System::Drawing::Size(180, 35);
            txtSpecialization->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);

            Label^ lblPhone = gcnew Label();
            lblPhone->Text = L"Phone:*";
            lblPhone->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblPhone->Location = Point(660, 55);
            lblPhone->Size = System::Drawing::Size(70, 25);

            txtPhone = gcnew TextBox();
            txtPhone->Location = Point(735, 52);
            txtPhone->Size = System::Drawing::Size(160, 35);
            txtPhone->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);

            Label^ lblEmail = gcnew Label();
            lblEmail->Text = L"Email:";
            lblEmail->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblEmail->Location = Point(15, 105);
            lblEmail->Size = System::Drawing::Size(70, 25);

            txtEmail = gcnew TextBox();
            txtEmail->Location = Point(90, 102);
            txtEmail->Size = System::Drawing::Size(280, 35);
            txtEmail->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);

            btnAdd = CreateButton(L"➕ Add Doctor", Color::FromArgb(46, 204, 113), 15, 158, 130, 42);
            btnUpdate = CreateButton(L"✏️ Update", Color::FromArgb(52, 152, 219), 160, 158, 110, 42);
            btnDelete = CreateButton(L"🗑️ Delete", Color::FromArgb(231, 76, 60), 285, 158, 110, 42);
            btnClear = CreateButton(L"🔄 Clear", Color::FromArgb(149, 165, 166), 410, 158, 110, 42);

            btnAdd->Click += gcnew EventHandler(this, &Doctors::btnAdd_Click);
            btnUpdate->Click += gcnew EventHandler(this, &Doctors::btnUpdate_Click);
            btnDelete->Click += gcnew EventHandler(this, &Doctors::btnDelete_Click);
            btnClear->Click += gcnew EventHandler(this, &Doctors::btnClear_Click);

            inputPanel->Controls->Add(lblFormTitle);
            inputPanel->Controls->Add(lblName); inputPanel->Controls->Add(txtName);
            inputPanel->Controls->Add(lblSpecialization); inputPanel->Controls->Add(txtSpecialization);
            inputPanel->Controls->Add(lblPhone); inputPanel->Controls->Add(txtPhone);
            inputPanel->Controls->Add(lblEmail); inputPanel->Controls->Add(txtEmail);
            inputPanel->Controls->Add(btnAdd); inputPanel->Controls->Add(btnUpdate);
            inputPanel->Controls->Add(btnDelete); inputPanel->Controls->Add(btnClear);

            mainContentPanel->Controls->Add(topBar);
            mainContentPanel->Controls->Add(dgvDoctors);
            mainContentPanel->Controls->Add(inputPanel);

            contentPanel->Controls->Add(statsPanel);
            contentPanel->Controls->Add(mainContentPanel);

            this->Controls->Add(contentPanel);
            this->Controls->Add(headerPanel);

            timerDateTime = gcnew Timer();
            timerDateTime->Interval = 1000;
            timerDateTime->Tick += gcnew EventHandler(this, &Doctors::timerDateTime_Tick);
            timerDateTime->Start();

            this->Resize += gcnew EventHandler(this, &Doctors::OnResize);
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

            cli::array<String^>^ titles = gcnew cli::array<String^>(4) { L"Total Doctors", L"Departments", L"On Leave", L"Available Today" };
            cli::array<String^>^ values = gcnew cli::array<String^>(4) { doctorsList->Count.ToString(), L"12", L"3", L"28" };
            cli::array<String^>^ icons = gcnew cli::array<String^>(4) { L"👨‍⚕️", L"🏥", L"🚪", L"✅" };
            cli::array<Color>^ colors = gcnew cli::array<Color>(4) {
                Color::FromArgb(46, 204, 113),
                Color::FromArgb(52, 152, 219),
                Color::FromArgb(231, 76, 60),
                Color::FromArgb(241, 196, 15)
            };

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
                iconCircle->BackColor = Color::FromArgb(213, 247, 218);

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
            AddDoctor(L"Dr. Mohamed Ali", L"Cardiology", L"01001122334", L"mohamed@hospital.com");
            AddDoctor(L"Dr. Sara Hassan", L"Pediatrics", L"01004455667", L"sara@hospital.com");
            AddDoctor(L"Dr. Ahmed Kamel", L"Neurology", L"01007788990", L"ahmed@hospital.com");
            AddDoctor(L"Dr. Nadia Fathy", L"Dermatology", L"01002233445", L"nadia@hospital.com");
        }

        void AddDoctor(String^ name, String^ specialization, String^ phone, String^ email) {
            Dictionary<String^, String^>^ doctor = gcnew Dictionary<String^, String^>();
            doctor->Add(L"ID", (nextId++).ToString()); doctor->Add(L"Name", name);
            doctor->Add(L"Specialization", specialization); doctor->Add(L"Phone", phone);
            doctor->Add(L"Email", email); doctorsList->Add(doctor);
        }

        void RefreshGrid() {
            dgvDoctors->Columns->Clear();
            dgvDoctors->Rows->Clear();

            // تعريف الأعمدة
            dgvDoctors->Columns->Add(L"ID", L"ID");
            dgvDoctors->Columns->Add(L"Name", L"Doctor Name");
            dgvDoctors->Columns->Add(L"Specialization", L"Specialization");
            dgvDoctors->Columns->Add(L"Phone", L"Phone");
            dgvDoctors->Columns->Add(L"Email", L"Email");

            DbConnection db;
            sql::Connection* conn = db.getConnect();

            if (conn != nullptr) {
                try {
                    sql::Statement* stmt = conn->createStatement();
                    // تأكدي أن اسم الجدول في MySQL هو 'doctors'
                    sql::ResultSet* res = stmt->executeQuery("SELECT * FROM doctors");

                    while (res->next()) {
                        dgvDoctors->Rows->Add(
                            res->getInt("id").ToString(),
                            gcnew String(res->getString("name").c_str()),
                            gcnew String(res->getString("specialization").c_str()),
                            gcnew String(res->getString("phone").c_str()),
                            gcnew String(res->getString("email").c_str())
                        );
                    }

                    delete res;
                    delete stmt;
                    delete conn;
                }
                catch (sql::SQLException& e) {
                    MessageBox::Show(L"Error connecting to database!", L"DB Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                }
            }
            LoadStatsCards(); // عشان يحدث الأرقام اللي في الكروت فوق
        }

        void SearchDoctors(String^ searchTerm) {
            dgvDoctors->Rows->Clear();
            for each (Dictionary<String^, String^> ^ doctor in doctorsList) {
                if (doctor[L"Name"]->ToLower()->Contains(searchTerm->ToLower()) ||
                    doctor[L"Specialization"]->ToLower()->Contains(searchTerm->ToLower())) {
                    dgvDoctors->Rows->Add(doctor[L"ID"], doctor[L"Name"], doctor[L"Specialization"],
                        doctor[L"Phone"], doctor[L"Email"]);
                }
            }
        }

        void LoadStyles() {
            dgvDoctors->EnableHeadersVisualStyles = false;
            dgvDoctors->ColumnHeadersDefaultCellStyle->BackColor = Color::FromArgb(46, 204, 113);
            dgvDoctors->ColumnHeadersDefaultCellStyle->ForeColor = Color::White;
            dgvDoctors->ColumnHeadersDefaultCellStyle->Font = gcnew System::Drawing::Font(L"Segoe UI", 11, FontStyle::Bold);
            dgvDoctors->AlternatingRowsDefaultCellStyle->BackColor = Color::FromArgb(248, 249, 250);
            ApplyRoundedRegion(mainContentPanel, 12);
        }

        void OnResize(Object^ sender, EventArgs^ e) {
            lblDateTime->Location = Point(this->ClientSize.Width - 340, 22);
            notificationPanel->Location = Point(this->ClientSize.Width - 170, 14);
            pbAvatar->Location = Point(this->ClientSize.Width - 100, 11);
        }

        void timerDateTime_Tick(Object^ sender, EventArgs^ e) { UpdateDateTime(); }
        void UpdateDateTime() { lblDateTime->Text = DateTime::Now.ToString(L"dddd, MMMM dd, yyyy  hh:mm:ss tt"); }
        void ScrollToInput(Object^ sender, EventArgs^ e) { txtName->Focus(); }

        void txtSearch_Enter(Object^ sender, EventArgs^ e) {
            if (txtSearch->Text == L"Search by name or specialization...") {
                txtSearch->Text = L""; txtSearch->ForeColor = Color::Black;
            }
        }

        void txtSearch_Leave(Object^ sender, EventArgs^ e) {
            if (txtSearch->Text->Length == 0) {
                txtSearch->Text = L"Search by name or specialization...";
                txtSearch->ForeColor = Color::FromArgb(150, 150, 150);
            }
        }

        void txtSearch_TextChanged(Object^ sender, EventArgs^ e) {
            if (txtSearch->Text != L"Search by name or specialization..." && txtSearch->Text->Length > 0)
                SearchDoctors(txtSearch->Text);
            else RefreshGrid();
        }

        void dgvDoctors_CellClick(Object^ sender, DataGridViewCellEventArgs^ e) {
            if (e->RowIndex >= 0) {
                DataGridViewRow^ row = dgvDoctors->Rows[e->RowIndex];
                txtName->Text = row->Cells[1]->Value->ToString();
                txtSpecialization->Text = row->Cells[2]->Value->ToString();
                txtPhone->Text = row->Cells[3]->Value->ToString();
                txtEmail->Text = row->Cells[4]->Value->ToString();
            }
        }

        void btnAdd_Click(Object^ sender, EventArgs^ e) {
            if (txtName->Text->Length > 0 && txtSpecialization->Text->Length > 0) {
                DbConnection db;
                sql::Connection* conn = db.getConnect();

                if (conn != nullptr) {
                    try {
                        sql::PreparedStatement* pstmt = conn->prepareStatement(
                            "INSERT INTO doctors (name, specialization, phone, email) VALUES (?, ?, ?, ?)"
                        );

                        // تحويل Managed String (System::String^) إلى Native String (std::string)
                        using namespace System::Runtime::InteropServices;
                        const char* nameChars = (const char*)(Marshal::StringToHGlobalAnsi(txtName->Text)).ToPointer();
                        const char* specChars = (const char*)(Marshal::StringToHGlobalAnsi(txtSpecialization->Text)).ToPointer();
                        const char* phoneChars = (const char*)(Marshal::StringToHGlobalAnsi(txtPhone->Text)).ToPointer();
                        const char* emailChars = (const char*)(Marshal::StringToHGlobalAnsi(txtEmail->Text)).ToPointer();

                        pstmt->setString(1, nameChars);
                        pstmt->setString(2, specChars);
                        pstmt->setString(3, phoneChars);
                        pstmt->setString(4, emailChars);

                        pstmt->executeUpdate();

                        // تنظيف الـ Memory من الـ pointers
                        Marshal::FreeHGlobal(IntPtr((void*)nameChars));
                        Marshal::FreeHGlobal(IntPtr((void*)specChars));
                        Marshal::FreeHGlobal(IntPtr((void*)phoneChars));
                        Marshal::FreeHGlobal(IntPtr((void*)emailChars));

                        delete pstmt;
                        delete conn;

                        RefreshGrid();
                        ClearFields();
                        MessageBox::Show(L"✓ Doctor added to Database successfully!", L"Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
                    }
                    catch (sql::SQLException& e) {
                        MessageBox::Show(L"Database Error: " + gcnew String(e.what()), L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                    }
                }
            }
            else MessageBox::Show(L"Please fill required fields", L"Error", MessageBoxButtons::OK, MessageBoxIcon::Warning);
        }

        void btnUpdate_Click(Object^ sender, EventArgs^ e) {
            if (dgvDoctors->SelectedRows->Count > 0) {
                int index = dgvDoctors->SelectedRows[0]->Index;
                if (index < doctorsList->Count) {
                    doctorsList[index]->default[L"Name"] = txtName->Text;
                    doctorsList[index]->default[L"Specialization"] = txtSpecialization->Text;
                    doctorsList[index]->default[L"Phone"] = txtPhone->Text;
                    doctorsList[index]->default[L"Email"] = txtEmail->Text;
                    RefreshGrid(); ClearFields();
                    MessageBox::Show(L"✓ Doctor updated successfully!", L"Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
                }
            }
            else MessageBox::Show(L"Please select a doctor to update", L"Info", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }

        void btnDelete_Click(Object^ sender, EventArgs^ e) {
            if (dgvDoctors->SelectedRows->Count > 0) {
                String^ doctorID = dgvDoctors->SelectedRows[0]->Cells[0]->Value->ToString();

                if (MessageBox::Show(L"Are you sure you want to delete Doctor ID: " + doctorID + L"?", L"Confirm Delete", MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {

                    DbConnection db;
                    sql::Connection* conn = db.getConnect();

                    if (conn != nullptr) {
                        try {
                            sql::PreparedStatement* pstmt = conn->prepareStatement("DELETE FROM doctors WHERE id = ?");
                            pstmt->setInt(1, System::Int32::Parse(doctorID));
                            pstmt->executeUpdate();

                            delete pstmt;
                            delete conn;

                            RefreshGrid();
                            ClearFields();
                        }
                        catch (sql::SQLException& e) {
                            MessageBox::Show(L"Could not delete!", L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                        }
                    }
                }
            }
        }
        void btnClear_Click(Object^ sender, EventArgs^ e) { ClearFields(); }
        void ClearFields() { txtName->Clear(); txtSpecialization->Clear(); txtPhone->Clear(); txtEmail->Clear(); }
    };
}
