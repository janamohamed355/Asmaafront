#pragma once
#include "DbConnection.h"
#include "Patients.h"
#include "Doctors.h"
#include "Appointments.h"
#include "Pharmacy.h"
#include "Invoices.h"
#include "Rooms.h"

namespace HospitalManagementSystem {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Drawing::Drawing2D;

    public ref class Dashboard : public Form {
    private:
        Panel^ sidePanel;
        Panel^ headerPanel;
        Panel^ contentPanel;
        Panel^ statsPanel;
        cli::array<Panel^>^ statCards;
        Button^ btnPatients; Button^ btnDoctors; Button^ btnAppointments;
        Button^ btnPharmacy; Button^ btnInvoices; Button^ btnRooms; Button^ btnLogout;
        Label^ lblWelcome; Label^ lblDateTime; PictureBox^ pbAvatar;
        Panel^ notificationPanel; Label^ lblNotificationCount;
        Timer^ timerDateTime; Timer^ timerFade;
        String^ userRole; String^ userName;
        float opacity = 0.0f;

    public:
        Dashboard(String^ role, String^ username) {
            userRole = role; userName = username;
            InitializeComponent(); LoadStyles(); SetupAnimations();
            UpdateDateTime(); LoadStatistics(); UpdateMenuByRole();
            ApplySpacing();
        }

    protected:
        ~Dashboard() {
            if (timerDateTime != nullptr) delete timerDateTime;
            if (timerFade != nullptr) delete timerFade;
        }

    private:
        // --- معالجات الأحداث الجديدة (بدلاً من الـ Lambdas) ---

        void OnAvatarPaint(Object^ s, PaintEventArgs^ e) {
            Graphics^ g = e->Graphics;
            g->SmoothingMode = SmoothingMode::AntiAlias;
            PictureBox^ avatar = safe_cast<PictureBox^>(s);
            g->FillEllipse(gcnew SolidBrush(Color::FromArgb(52, 152, 219)), avatar->ClientRectangle);
            String^ initial = userRole == L"Admin" ? L"A" : L"S";
            System::Drawing::Font^ font = gcnew System::Drawing::Font(L"Segoe UI", 18, FontStyle::Bold);
            SizeF textSize = g->MeasureString(initial, font);
            g->DrawString(initial, font, gcnew SolidBrush(Color::White),
                (50 - textSize.Width) / 2, (50 - textSize.Height) / 2);
            delete font;
        }

        void OnNotificationPaint(Object^ s, PaintEventArgs^ e) {
            Control^ ctrl = safe_cast<Control^>(s);
            System::Drawing::Drawing2D::GraphicsPath^ path = gcnew System::Drawing::Drawing2D::GraphicsPath();
            path->AddEllipse(0, 0, ctrl->Width, ctrl->Height);
            ctrl->Region = gcnew System::Drawing::Region(path);
            delete path;
        }

        void OnPbAvatarPaint(Object^ s, PaintEventArgs^ e) {
            Graphics^ g = e->Graphics;
            g->SmoothingMode = SmoothingMode::AntiAlias;
            g->FillEllipse(gcnew SolidBrush(Color::FromArgb(52, 152, 219)), pbAvatar->ClientRectangle);
            if (userName != nullptr && userName->Length > 0) {
                String^ initial = userName->Substring(0, 1)->ToUpper();
                System::Drawing::Font^ font = gcnew System::Drawing::Font(L"Segoe UI", 16, FontStyle::Bold);
                SizeF textSize = g->MeasureString(initial, font);
                g->DrawString(initial, font, gcnew SolidBrush(Color::White),
                    (48 - textSize.Width) / 2, (48 - textSize.Height) / 2);
                delete font;
            }
        }

        void OnCardMouseEnter(Object^ s, EventArgs^ e) {
            Panel^ card = safe_cast<Panel^>(s);
            card->BackColor = Color::FromArgb(250, 251, 252);
            card->Cursor = Cursors::Hand;
        }

        void OnCardMouseLeave(Object^ s, EventArgs^ e) {
            Panel^ card = safe_cast<Panel^>(s);
            card->BackColor = Color::White;
        }

        void OnSidePanelPaint(Object^ s, PaintEventArgs^ e) {
            Panel^ p = safe_cast<Panel^>(s);
            System::Drawing::Rectangle rect = p->ClientRectangle;
            LinearGradientBrush^ brush = gcnew LinearGradientBrush(rect,
                Color::FromArgb(25, 35, 55),
                Color::FromArgb(35, 45, 65), 90.0f);
            e->Graphics->FillRectangle(brush, rect);
            delete brush;
        }

        void OnHeaderPanelPaint(Object^ s, PaintEventArgs^ e) {
            Panel^ p = safe_cast<Panel^>(s);
            ControlPaint::DrawBorder(e->Graphics, p->ClientRectangle,
                Color::FromArgb(220, 220, 230), 0, ButtonBorderStyle::Solid,
                Color::FromArgb(220, 220, 230), 0, ButtonBorderStyle::Solid,
                Color::FromArgb(220, 220, 230), 0, ButtonBorderStyle::Solid,
                Color::FromArgb(220, 220, 230), 1, ButtonBorderStyle::Solid);
        }

        void OnIconCirclePaint(Object^ s, PaintEventArgs^ e) {
            Panel^ p = safe_cast<Panel^>(s);
            Graphics^ g = e->Graphics;
            g->SmoothingMode = SmoothingMode::AntiAlias;
            Color clr = Color::FromArgb(52, 152, 219);
            if (p->Tag != nullptr) {
                // Tag holds a boxed System::Drawing::Color
                clr = safe_cast<Color>(p->Tag);
            }
            SolidBrush^ brush = gcnew SolidBrush(clr);
            g->FillEllipse(brush, 0, 0, p->Width, p->Height);
            delete brush;
        }

        // ---------------------------------------------------------

        void InitializeComponent() {
            this->Text = L"CareHub - Hospital Management System";
            this->Size = System::Drawing::Size(1350, 800);
            this->StartPosition = FormStartPosition::CenterScreen;
            this->WindowState = FormWindowState::Maximized;
            this->BackColor = Color::FromArgb(240, 242, 245);
            this->Opacity = 0;
            this->Resize += gcnew EventHandler(this, &Dashboard::OnResize);

            sidePanel = gcnew Panel();
            sidePanel->Dock = DockStyle::Left;
            sidePanel->Width = 280;
            sidePanel->BackColor = Color::FromArgb(25, 35, 55);

            Panel^ logoPanel = gcnew Panel();
            logoPanel->Height = 140; logoPanel->Dock = DockStyle::Top;
            logoPanel->BackColor = Color::FromArgb(35, 45, 65);

            Label^ logoIcon = gcnew Label();
            logoIcon->Text = L"🏥";
            logoIcon->Font = gcnew System::Drawing::Font(L"Segoe UI", 48);
            logoIcon->ForeColor = Color::FromArgb(52, 152, 219);
            logoIcon->TextAlign = ContentAlignment::MiddleCenter;
            logoIcon->Location = Point(0, 20);
            logoIcon->Size = System::Drawing::Size(280, 60);

            Label^ logoText = gcnew Label();
            logoText->Text = L"CAREHUB";
            logoText->Font = gcnew System::Drawing::Font(L"Segoe UI", 20, FontStyle::Bold);
            logoText->ForeColor = Color::White;
            logoText->TextAlign = ContentAlignment::MiddleCenter;
            logoText->Location = Point(0, 80);
            logoText->Size = System::Drawing::Size(280, 40);

            Label^ logoSub = gcnew Label();
            logoSub->Text = L"Hospital Management System";
            logoSub->Font = gcnew System::Drawing::Font(L"Segoe UI", 9);
            logoSub->ForeColor = Color::FromArgb(150, 150, 170);
            logoSub->Location = Point(0, 115);
            logoSub->Size = System::Drawing::Size(280, 25);

            logoPanel->Controls->Add(logoIcon);
            logoPanel->Controls->Add(logoText);
            logoPanel->Controls->Add(logoSub);

            Label^ menuTitle = gcnew Label();
            menuTitle->Text = L"MAIN MENU";
            menuTitle->Font = gcnew System::Drawing::Font(L"Segoe UI", 9, FontStyle::Bold);
            menuTitle->ForeColor = Color::FromArgb(100, 110, 140);
            menuTitle->Location = Point(25, 160);
            menuTitle->Size = System::Drawing::Size(230, 25);

            btnPatients = CreateMenuButton(L"👥 Patients", 0, Color::FromArgb(52, 152, 219));
            btnDoctors = CreateMenuButton(L"👨‍⚕️ Doctors", 1, Color::FromArgb(46, 204, 113));
            btnAppointments = CreateMenuButton(L"📅 Appointments", 2, Color::FromArgb(155, 89, 182));
            btnPharmacy = CreateMenuButton(L"💊 Pharmacy", 3, Color::FromArgb(241, 196, 15));
            btnInvoices = CreateMenuButton(L"💰 Invoices", 4, Color::FromArgb(230, 126, 34));
            btnRooms = CreateMenuButton(L"🛏️ Rooms", 5, Color::FromArgb(231, 76, 60));

            Panel^ userProfilePanel = gcnew Panel();
            userProfilePanel->Dock = DockStyle::Bottom;
            userProfilePanel->Height = 100;
            userProfilePanel->BackColor = Color::FromArgb(35, 45, 65);

            PictureBox^ userAvatar = gcnew PictureBox();
            userAvatar->Location = Point(20, 15);
            userAvatar->Size = System::Drawing::Size(50, 50);
            userAvatar->BackColor = Color::FromArgb(52, 152, 219);
            userAvatar->Paint += gcnew PaintEventHandler(this, &Dashboard::OnAvatarPaint);

            Label^ lblUserName = gcnew Label();
            lblUserName->Text = userName;
            lblUserName->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblUserName->ForeColor = Color::White;
            lblUserName->Location = Point(85, 20);
            lblUserName->Size = System::Drawing::Size(180, 25);

            Label^ lblUserRole = gcnew Label();
            lblUserRole->Text = userRole == L"Admin" ? L"System Administrator" : L"Medical Staff";
            lblUserRole->Font = gcnew System::Drawing::Font(L"Segoe UI", 8);
            lblUserRole->ForeColor = Color::FromArgb(150, 150, 170);
            lblUserRole->Location = Point(85, 45);
            lblUserRole->Size = System::Drawing::Size(180, 20);

            userProfilePanel->Controls->Add(userAvatar);
            userProfilePanel->Controls->Add(lblUserName);
            userProfilePanel->Controls->Add(lblUserRole);

            btnLogout = CreateMenuButton(L"🚪 Logout", 10, Color::FromArgb(231, 76, 60));
            btnLogout->Click += gcnew EventHandler(this, &Dashboard::Logout);

            sidePanel->Controls->Add(logoPanel);
            sidePanel->Controls->Add(menuTitle);
            sidePanel->Controls->Add(btnPatients);
            sidePanel->Controls->Add(btnDoctors);
            sidePanel->Controls->Add(btnAppointments);
            sidePanel->Controls->Add(btnPharmacy);
            sidePanel->Controls->Add(btnInvoices);
            sidePanel->Controls->Add(btnRooms);
            sidePanel->Controls->Add(userProfilePanel);
            sidePanel->Controls->Add(btnLogout);

            headerPanel = gcnew Panel();
            headerPanel->Dock = DockStyle::Top;
            headerPanel->Height = 70;
            headerPanel->BackColor = Color::White;
            headerPanel->Padding = System::Windows::Forms::Padding(20, 0, 20, 0);

            lblWelcome = gcnew Label();
            lblWelcome->Text = L"Good " + GetTimeOfDay() + L", " + userName + L"!";
            lblWelcome->Font = gcnew System::Drawing::Font(L"Segoe UI", 16, FontStyle::Bold);
            lblWelcome->ForeColor = Color::FromArgb(44, 62, 80);
            lblWelcome->Location = Point(20, 18);
            lblWelcome->Size = System::Drawing::Size(400, 40);

            lblDateTime = gcnew Label();
            lblDateTime->Font = gcnew System::Drawing::Font(L"Segoe UI", 10);
            lblDateTime->ForeColor = Color::FromArgb(100, 110, 140);
            lblDateTime->TextAlign = ContentAlignment::MiddleRight;
            lblDateTime->Size = System::Drawing::Size(300, 25);
            lblDateTime->Anchor = AnchorStyles::Top | AnchorStyles::Right;

            notificationPanel = gcnew Panel();
            notificationPanel->Size = System::Drawing::Size(42, 42);
            notificationPanel->BackColor = Color::FromArgb(248, 249, 250);
            notificationPanel->Cursor = Cursors::Hand;
            notificationPanel->Anchor = AnchorStyles::Top | AnchorStyles::Right;
            notificationPanel->Paint += gcnew PaintEventHandler(this, &Dashboard::OnNotificationPaint);

            Label^ bellIcon = gcnew Label();
            bellIcon->Text = L"🔔";
            bellIcon->Font = gcnew System::Drawing::Font(L"Segoe UI", 18);
            bellIcon->Location = Point(10, 8);
            bellIcon->Size = System::Drawing::Size(28, 28);
            bellIcon->TextAlign = ContentAlignment::MiddleCenter;

            lblNotificationCount = gcnew Label();
            lblNotificationCount->Text = L"3";
            lblNotificationCount->Font = gcnew System::Drawing::Font(L"Segoe UI", 8, FontStyle::Bold);
            lblNotificationCount->ForeColor = Color::White;
            lblNotificationCount->BackColor = Color::FromArgb(231, 76, 60);
            lblNotificationCount->Location = Point(26, 6);
            lblNotificationCount->Size = System::Drawing::Size(18, 18);
            lblNotificationCount->TextAlign = ContentAlignment::MiddleCenter;

            notificationPanel->Controls->Add(bellIcon);
            notificationPanel->Controls->Add(lblNotificationCount);

            pbAvatar = gcnew PictureBox();
            pbAvatar->Size = System::Drawing::Size(48, 48);
            pbAvatar->BackColor = Color::FromArgb(52, 152, 219);
            pbAvatar->Anchor = AnchorStyles::Top | AnchorStyles::Right;
            pbAvatar->Paint += gcnew PaintEventHandler(this, &Dashboard::OnPbAvatarPaint);

            headerPanel->Controls->Add(lblWelcome);
            headerPanel->Controls->Add(lblDateTime);
            headerPanel->Controls->Add(notificationPanel);
            headerPanel->Controls->Add(pbAvatar);

            contentPanel = gcnew Panel();
            contentPanel->Dock = DockStyle::Fill;
            contentPanel->BackColor = Color::FromArgb(240, 242, 245);
            contentPanel->Padding = System::Windows::Forms::Padding(300, 20, 25, 20);
            contentPanel->AutoScroll = true;

            statsPanel = gcnew Panel();
            statsPanel->Location = Point(0, 0);
            statsPanel->Size = System::Drawing::Size(1050, 200);
            statsPanel->Anchor = AnchorStyles::Left | AnchorStyles::Top | AnchorStyles::Right;
            statsPanel->BackColor = Color::Transparent;

            contentPanel->Controls->Add(statsPanel);

            this->Controls->Add(headerPanel);
            this->Controls->Add(sidePanel);
            this->Controls->Add(contentPanel);

            timerDateTime = gcnew Timer();
            timerDateTime->Interval = 1000;
            timerDateTime->Tick += gcnew EventHandler(this, &Dashboard::timerDateTime_Tick);
            timerDateTime->Start();
        }

        void ApplySpacing() {
            lblDateTime->Location = Point(this->ClientSize.Width - 320, 22);
            notificationPanel->Location = Point(this->ClientSize.Width - 160, 14);
            pbAvatar->Location = Point(this->ClientSize.Width - 95, 11);
        }

        String^ GetTimeOfDay() {
            int hour = DateTime::Now.Hour;
            if (hour < 12) return L"Morning";
            if (hour < 18) return L"Afternoon";
            return L"Evening";
        }

        Button^ CreateMenuButton(String^ text, int index, Color color) {
            Button^ btn = gcnew Button();
            btn->Text = text;
            btn->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Regular);
            btn->FlatStyle = FlatStyle::Flat;
            btn->ForeColor = Color::White;
            btn->TextAlign = ContentAlignment::MiddleLeft;
            btn->Height = 48;
            btn->Width = 280;
            btn->Location = Point(0, 195 + (index * 52));
            btn->BackColor = Color::FromArgb(25, 35, 55);
            btn->FlatAppearance->MouseOverBackColor = color;
            btn->Cursor = Cursors::Hand;

            if (text->Contains(L"Patients")) btn->Click += gcnew EventHandler(this, &Dashboard::ShowPatients);
            else if (text->Contains(L"Doctors")) btn->Click += gcnew EventHandler(this, &Dashboard::ShowDoctors);
            else if (text->Contains(L"Appointments")) btn->Click += gcnew EventHandler(this, &Dashboard::ShowAppointments);
            else if (text->Contains(L"Pharmacy")) btn->Click += gcnew EventHandler(this, &Dashboard::ShowPharmacy);
            else if (text->Contains(L"Invoices")) btn->Click += gcnew EventHandler(this, &Dashboard::ShowInvoices);
            else if (text->Contains(L"Rooms")) btn->Click += gcnew EventHandler(this, &Dashboard::ShowRooms);

            return btn;
        }

        void LoadStatistics() {
            statsPanel->Controls->Clear();

            // متغيرات لتخزين الأرقام من الداتا بيز
            int pCount = 0;
            int dCount = 0;
            int pendingInv = 0;
            double totalRevenue = 0.0;

            // --- الربط مع الداتا بيز وجلب البيانات ---
            DbConnection db;
            sql::Connection* conn = db.getConnect();

            if (conn != nullptr) {
                try {
                    sql::Statement* stmt = conn->createStatement();
                    sql::ResultSet* res;

                    // 1. عدد المرضى
                    res = stmt->executeQuery("SELECT COUNT(*) AS total FROM patients");
                    if (res->next()) pCount = res->getInt("total");

                    // 2. عدد الدكاترة
                    res = stmt->executeQuery("SELECT COUNT(*) AS total FROM doctors");
                    if (res->next()) dCount = res->getInt("total");

                    // 3. الفواتير المعلقة (Pending)
                    res = stmt->executeQuery("SELECT COUNT(*) AS total FROM invoices WHERE status = 'Pending'");
                    if (res->next()) pendingInv = res->getInt("total");

                    // 4. إجمالي الإيرادات (Revenue)
                    res = stmt->executeQuery("SELECT SUM(amount) AS total FROM invoices WHERE status = 'Paid'");
                    if (res->next()) totalRevenue = res->getDouble("total");

                    delete res;
                    delete stmt;
                    delete conn;
                }
                catch (sql::SQLException& e) {
                    // في حالة وجود خطأ، الأرقام هتفضل 0
                }
            }

            // المصفوفات اللي بتعرض البيانات في الكروت (استخدمنا المتغيرات اللي فوق)
            cli::array<String^>^ titles = gcnew cli::array<String^>(6) {
                L"Total Patients", L"Doctors", L"Pending Invoices", L"Available Rooms", L"Monthly Revenue", L"Active Cases"
            };
            cli::array<String^>^ values = gcnew cli::array<String^>(6) {
                pCount.ToString(),
                    dCount.ToString(),
                    pendingInv.ToString(),
                    L"24",
                    L"$" + totalRevenue.ToString("N2"),
                    L"5"
            };

            cli::array<String^>^ icons = gcnew cli::array<String^>(6) { L"👥", L"👨‍⚕️", L"📄", L"🛏️", L"💰", L"🏥" };
            cli::array<Color>^ colors = gcnew cli::array<Color>(6) {
                Color::FromArgb(52, 152, 219), Color::FromArgb(46, 204, 113),
                    Color::FromArgb(230, 126, 34), Color::FromArgb(241, 196, 15),
                    Color::FromArgb(52, 73, 94), Color::FromArgb(231, 76, 60)
            };

            // --- الجزء المسؤول عن رسم الكروت (نفس اللي كان عندك) ---
            int cardWidth = (statsPanel->Width - 30) / 3;
            if (cardWidth < 280) cardWidth = 280;

            int row = 0, col = 0;
            for (int i = 0; i < 6; i++) {
                Panel^ card = gcnew Panel();
                card->Size = System::Drawing::Size(cardWidth - 15, 145);
                card->Location = Point(10 + (col * cardWidth), 10 + (row * 160));
                card->BackColor = Color::White;
                card->Anchor = AnchorStyles::Left | AnchorStyles::Top;
                card->MouseEnter += gcnew EventHandler(this, &Dashboard::OnCardMouseEnter);
                card->MouseLeave += gcnew EventHandler(this, &Dashboard::OnCardMouseLeave);

                // Icon circle
                Panel^ iconCircle = gcnew Panel();
                iconCircle->Size = System::Drawing::Size(48, 48);
                iconCircle->Location = Point(10, 15);
                iconCircle->BackColor = colors[i];
                // Store color in Tag so the member paint handler can access it
                iconCircle->Tag = colors[i];
                iconCircle->Paint += gcnew PaintEventHandler(this, &Dashboard::OnIconCirclePaint);

                Label^ lblIcon = gcnew Label();
                lblIcon->Text = icons[i];
                lblIcon->Font = gcnew System::Drawing::Font(L"Segoe UI", 18);
                lblIcon->Size = System::Drawing::Size(48, 48);
                lblIcon->Location = Point(10, 15);
                lblIcon->TextAlign = ContentAlignment::MiddleCenter;
                lblIcon->BackColor = Color::Transparent;

                Label^ lblTitle = gcnew Label();
                lblTitle->Text = titles[i];
                lblTitle->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Regular);
                lblTitle->ForeColor = Color::FromArgb(120, 130, 150);
                lblTitle->Location = Point(70, 20);
                lblTitle->Size = System::Drawing::Size(card->Width - 80, 20);

                Label^ lblValue = gcnew Label();
                lblValue->Text = values[i];
                lblValue->Font = gcnew System::Drawing::Font(L"Segoe UI", 16, FontStyle::Bold);
                lblValue->ForeColor = Color::FromArgb(44, 62, 80);
                lblValue->Location = Point(70, 45);
                lblValue->Size = System::Drawing::Size(card->Width - 80, 35);

                card->Controls->Add(iconCircle);
                card->Controls->Add(lblIcon);
                card->Controls->Add(lblTitle);
                card->Controls->Add(lblValue);

                statsPanel->Controls->Add(card);
                col++;
                if (col >= 3) { col = 0; row++; }
            }
        }

        void UpdateMenuByRole() {
            if (userRole == L"Staff") {
                btnInvoices->Visible = false;
                btnRooms->Visible = false;
            }
        }

        void ShowPatients(Object^ sender, EventArgs^ e) { ClearAndShowForm(gcnew Patients()); }
        void ShowDoctors(Object^ sender, EventArgs^ e) { ClearAndShowForm(gcnew Doctors()); }
        void ShowAppointments(Object^ sender, EventArgs^ e) { ClearAndShowForm(gcnew Appointments()); }
        void ShowPharmacy(Object^ sender, EventArgs^ e) { ClearAndShowForm(gcnew Pharmacy()); }
        void ShowInvoices(Object^ sender, EventArgs^ e) { ClearAndShowForm(gcnew Invoices()); }
        void ShowRooms(Object^ sender, EventArgs^ e) { ClearAndShowForm(gcnew Rooms()); }

        void ClearAndShowForm(Form^ form) {
            contentPanel->Controls->Clear();
            form->TopLevel = false;
            form->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
            form->Dock = DockStyle::Fill;
            contentPanel->Controls->Add(form);
            form->Show();
        }

        void Logout(Object^ sender, EventArgs^ e) {
            System::Windows::Forms::DialogResult result = MessageBox::Show(L"Are you sure you want to logout?",
                L"Confirm Logout", MessageBoxButtons::YesNo, MessageBoxIcon::Question);
            if (result == System::Windows::Forms::DialogResult::Yes) {
                this->Close();
                Application::Restart();
            }
        }

        void LoadStyles() {
            sidePanel->Paint += gcnew PaintEventHandler(this, &Dashboard::OnSidePanelPaint);
            headerPanel->Paint += gcnew PaintEventHandler(this, &Dashboard::OnHeaderPanelPaint);
        }

        void SetupAnimations() {
            timerFade = gcnew Timer();
            timerFade->Interval = 20;
            timerFade->Tick += gcnew EventHandler(this, &Dashboard::FadeTick);
            timerFade->Start();
        }

        void FadeTick(Object^ sender, EventArgs^ e) {
            opacity += 0.05f;
            if (opacity >= 1.0f) {
                opacity = 1.0f;
                timerFade->Stop();
            }
            this->Opacity = (double)opacity;
        }

        void timerDateTime_Tick(Object^ sender, EventArgs^ e) { UpdateDateTime(); }
        void UpdateDateTime() { lblDateTime->Text = DateTime::Now.ToString(L"dddd, MMMM dd, yyyy  hh:mm:ss tt"); }
        void OnResize(Object^ sender, EventArgs^ e) { ApplySpacing(); }
    };
}
