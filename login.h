// Login.h - Ultimate Professional Version
#pragma once
#include "DbConnection.h"
#include <Windows.h>
#include <msclr/marshal_cppstd.h>

namespace HospitalManagementSystem {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Drawing::Drawing2D;
    using namespace System::ComponentModel;

    public ref class Login : public Form {
    private:
        Panel^ panelLeft;
        Panel^ panelRight;
        Panel^ panelForm;
        Button^ btnLogin;
        TextBox^ txtUsername;
        TextBox^ txtPassword;
        ComboBox^ cmbRole;
        Label^ lblWelcome;
        Label^ lblSubtitle;
        Label^ lblHospitalName;
        Label^ lblHospitalSlogan;
        LinkLabel^ llForgotPassword;
        Label^ lblDemoNote;
        Timer^ timerFade;
        float opacity = 0.0f;
        bool isDragging = false;
        Point dragStartPoint;

        // members for shake animation
        Timer^ shakeTimer;
        int shakeCount;
        System::Drawing::Point shakeOriginalLocation;

    public:
        property String^ UserRole;
        property String^ Username;

        Login() {
            InitializeComponent();
            LoadStyles();
            SetupAnimations();
        }

    protected:
        ~Login() {
            if (timerFade != nullptr) delete timerFade;
            if (shakeTimer != nullptr) delete shakeTimer;
        }

    private:
        void InitializeComponent() {
            this->Text = L"CareHub - Hospital Management System";
            this->Size = System::Drawing::Size(1000, 600);
            this->StartPosition = FormStartPosition::CenterScreen;
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
            this->BackColor = Color::White;
            this->Opacity = 0;

            // LEFT PANEL (Gradient)
            panelLeft = gcnew Panel();
            panelLeft->Dock = DockStyle::Left;
            panelLeft->Width = 450;

            lblHospitalName = gcnew Label();
            lblHospitalName->Text = L"🏥 CareHub";
            lblHospitalName->Font = gcnew System::Drawing::Font(L"Segoe UI", 34, FontStyle::Bold);
            lblHospitalName->ForeColor = Color::White;
            lblHospitalName->TextAlign = ContentAlignment::MiddleCenter;
            lblHospitalName->Location = Point(50, 170);
            lblHospitalName->Size = System::Drawing::Size(350, 70);

            lblHospitalSlogan = gcnew Label();
            lblHospitalSlogan->Text = L"Where Care Meets Excellence";
            lblHospitalSlogan->Font = gcnew System::Drawing::Font(L"Segoe UI", 14, FontStyle::Italic);
            lblHospitalSlogan->ForeColor = Color::FromArgb(220, 255, 255, 255);
            lblHospitalSlogan->TextAlign = ContentAlignment::MiddleCenter;
            lblHospitalSlogan->Location = Point(50, 250);
            lblHospitalSlogan->Size = System::Drawing::Size(350, 30);

            // Decorative line
            Panel^ linePanel = gcnew Panel();
            linePanel->BackColor = Color::FromArgb(100, 255, 255, 255);
            linePanel->Height = 2;
            linePanel->Width = 80;
            linePanel->Location = Point(185, 300);
            panelLeft->Controls->Add(linePanel);

            cli::array<String^>^ features = {
                L"✓ Patient Management", L"✓ Doctor Scheduling", L"✓ Pharmacy System",
                L"✓ Digital Invoicing", L"✓ Room Management", L"✓ 24/7 Support"
            };
            int yPos = 340;
            for (int i = 0; i < features->Length; i++) {
                Label^ lblFeature = gcnew Label();
                lblFeature->Text = features[i];
                lblFeature->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);
                lblFeature->ForeColor = Color::FromArgb(230, 255, 255, 255);
                lblFeature->Location = Point(80, yPos);
                lblFeature->Size = System::Drawing::Size(300, 28);
                panelLeft->Controls->Add(lblFeature);
                yPos += 38;
            }

            panelLeft->Controls->Add(lblHospitalName);
            panelLeft->Controls->Add(lblHospitalSlogan);

            // RIGHT PANEL
            panelRight = gcnew Panel();
            panelRight->Dock = DockStyle::Fill;
            panelRight->BackColor = Color::White;

            panelForm = gcnew Panel();
            panelForm->Size = System::Drawing::Size(420, 490);
            panelForm->Location = Point(540, 55);
            panelForm->BackColor = Color::White;

            lblWelcome = gcnew Label();
            lblWelcome->Text = L"Welcome Back";
            lblWelcome->Font = gcnew System::Drawing::Font(L"Segoe UI", 24, FontStyle::Bold);
            lblWelcome->ForeColor = Color::FromArgb(44, 62, 80);
            lblWelcome->Location = Point(30, 35);
            lblWelcome->Size = System::Drawing::Size(360, 50);

            lblSubtitle = gcnew Label();
            lblSubtitle->Text = L"Sign in to your account";
            lblSubtitle->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);
            lblSubtitle->ForeColor = Color::FromArgb(127, 140, 141);
            lblSubtitle->Location = Point(30, 85);
            lblSubtitle->Size = System::Drawing::Size(360, 25);

            Label^ lblRole = gcnew Label();
            lblRole->Text = L"Account Type";
            lblRole->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblRole->ForeColor = Color::FromArgb(52, 73, 94);
            lblRole->Location = Point(30, 135);
            lblRole->Size = System::Drawing::Size(200, 20);

            cmbRole = gcnew ComboBox();
            cmbRole->Location = Point(30, 157);
            cmbRole->Size = System::Drawing::Size(360, 35);
            cmbRole->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);
            cmbRole->DropDownStyle = ComboBoxStyle::DropDownList;
            cmbRole->FlatStyle = FlatStyle::Flat;
            cmbRole->BackColor = Color::FromArgb(248, 249, 250);
            cmbRole->Items->Add(L"🔑 Administrator");
            cmbRole->Items->Add(L"👨‍⚕️ Medical Staff");
            cmbRole->SelectedIndex = 0;

            Label^ lblUser = gcnew Label();
            lblUser->Text = L"Email Address";
            lblUser->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblUser->ForeColor = Color::FromArgb(52, 73, 94);
            lblUser->Location = Point(30, 210);
            lblUser->Size = System::Drawing::Size(200, 20);

            txtUsername = gcnew TextBox();
            txtUsername->Location = Point(30, 232);
            txtUsername->Size = System::Drawing::Size(360, 38);
            txtUsername->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);
            txtUsername->Text = L"admin@carehub.com";
            txtUsername->BorderStyle = BorderStyle::FixedSingle;
            txtUsername->BackColor = Color::FromArgb(248, 249, 250);
            txtUsername->Enter += gcnew EventHandler(this, &Login::txtUsername_Enter);
            txtUsername->Leave += gcnew EventHandler(this, &Login::txtUsername_Leave);

            Label^ lblPass = gcnew Label();
            lblPass->Text = L"Password";
            lblPass->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblPass->ForeColor = Color::FromArgb(52, 73, 94);
            lblPass->Location = Point(30, 290);
            lblPass->Size = System::Drawing::Size(200, 20);

            txtPassword = gcnew TextBox();
            txtPassword->Location = Point(30, 312);
            txtPassword->Size = System::Drawing::Size(360, 38);
            txtPassword->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);
            txtPassword->PasswordChar = L'•';
            txtPassword->Text = L"password123";
            txtPassword->BorderStyle = BorderStyle::FixedSingle;
            txtPassword->BackColor = Color::FromArgb(248, 249, 250);
            txtPassword->Enter += gcnew EventHandler(this, &Login::txtPassword_Enter);
            txtPassword->Leave += gcnew EventHandler(this, &Login::txtPassword_Leave);

            llForgotPassword = gcnew LinkLabel();
            llForgotPassword->Text = L"Forgot Password?";
            llForgotPassword->Font = gcnew System::Drawing::Font(L"Segoe UI", 9);
            llForgotPassword->ForeColor = Color::FromArgb(52, 152, 219);
            llForgotPassword->Location = Point(290, 360);
            llForgotPassword->Size = System::Drawing::Size(120, 20);
            llForgotPassword->LinkColor = Color::FromArgb(52, 152, 219);
            llForgotPassword->LinkClicked += gcnew LinkLabelLinkClickedEventHandler(this, &Login::llForgotPassword_LinkClicked);

            btnLogin = gcnew Button();
            btnLogin->Text = L"→ Sign In ←";
            btnLogin->Location = Point(30, 390);
            btnLogin->Size = System::Drawing::Size(360, 50);
            btnLogin->Font = gcnew System::Drawing::Font(L"Segoe UI", 12, FontStyle::Bold);
            btnLogin->BackColor = Color::FromArgb(52, 152, 219);
            btnLogin->ForeColor = Color::White;
            btnLogin->FlatStyle = FlatStyle::Flat;
            btnLogin->Cursor = Cursors::Hand;
            btnLogin->FlatAppearance->MouseOverBackColor = Color::FromArgb(41, 128, 185);
            btnLogin->Click += gcnew EventHandler(this, &Login::btnLogin_Click);

            lblDemoNote = gcnew Label();
            lblDemoNote->Text = L"💡 Demo: Any email/password works";
            lblDemoNote->Font = gcnew System::Drawing::Font(L"Segoe UI", 8);
            lblDemoNote->ForeColor = Color::FromArgb(149, 165, 166);
            lblDemoNote->Location = Point(120, 455);
            lblDemoNote->Size = System::Drawing::Size(250, 20);
            lblDemoNote->TextAlign = ContentAlignment::MiddleCenter;

            panelForm->Controls->Add(lblWelcome);
            panelForm->Controls->Add(lblSubtitle);
            panelForm->Controls->Add(lblRole);
            panelForm->Controls->Add(cmbRole);
            panelForm->Controls->Add(lblUser);
            panelForm->Controls->Add(txtUsername);
            panelForm->Controls->Add(lblPass);
            panelForm->Controls->Add(txtPassword);
            panelForm->Controls->Add(llForgotPassword);
            panelForm->Controls->Add(btnLogin);
            panelForm->Controls->Add(lblDemoNote);

            panelRight->Controls->Add(panelForm);
            this->Controls->Add(panelLeft);
            this->Controls->Add(panelRight);

            // Mouse drag
            this->MouseDown += gcnew MouseEventHandler(this, &Login::Form_MouseDown);
            this->MouseMove += gcnew MouseEventHandler(this, &Login::Form_MouseMove);
            this->MouseUp += gcnew MouseEventHandler(this, &Login::Form_MouseUp);
        }

        // ============ EVENT HANDLERS ============

        void Form_MouseDown(Object^ sender, MouseEventArgs^ e) {
            if (e->Button == System::Windows::Forms::MouseButtons::Left) {
                isDragging = true;
                dragStartPoint = Point(e->X, e->Y);
            }
        }

        void Form_MouseMove(Object^ sender, MouseEventArgs^ e) {
            if (isDragging) {
                Point diff = Point(e->X - dragStartPoint.X, e->Y - dragStartPoint.Y);
                this->Location = Point(this->Location.X + diff.X, this->Location.Y + diff.Y);
            }
        }

        void Form_MouseUp(Object^ sender, MouseEventArgs^ e) {
            isDragging = false;
        }

        void txtUsername_Enter(Object^ sender, EventArgs^ e) {
            txtUsername->BackColor = Color::White;
        }

        void txtUsername_Leave(Object^ sender, EventArgs^ e) {
            txtUsername->BackColor = Color::FromArgb(248, 249, 250);
        }

        void txtPassword_Enter(Object^ sender, EventArgs^ e) {
            txtPassword->BackColor = Color::White;
        }

        void txtPassword_Leave(Object^ sender, EventArgs^ e) {
            txtPassword->BackColor = Color::FromArgb(248, 249, 250);
        }

        // ============ HELPER METHODS ============

        System::Drawing::Drawing2D::GraphicsPath^ CreateRoundedRectangle(System::Drawing::Rectangle rect, int radius) {
            System::Drawing::Drawing2D::GraphicsPath^ path = gcnew System::Drawing::Drawing2D::GraphicsPath();
            path->AddArc(rect.X, rect.Y, radius * 2, radius * 2, 180, 90);
            path->AddArc(rect.Right - radius * 2, rect.Y, radius * 2, radius * 2, 270, 90);
            path->AddArc(rect.Right - radius * 2, rect.Bottom - radius * 2, radius * 2, radius * 2, 0, 90);
            path->AddArc(rect.X, rect.Bottom - radius * 2, radius * 2, radius * 2, 90, 90);
            path->CloseFigure();
            return path;
        }

        // ============ STYLES ============

        void LoadStyles() {
            panelLeft->Paint += gcnew PaintEventHandler(this, &Login::panelLeft_Paint);
            System::Drawing::Drawing2D::GraphicsPath^ path = CreateRoundedRectangle(this->ClientRectangle, 20);
            this->Region = gcnew System::Drawing::Region(path);
            delete path;
        }

        void panelLeft_Paint(Object^ sender, PaintEventArgs^ e) {
            System::Drawing::Rectangle rect = panelLeft->ClientRectangle;
            LinearGradientBrush^ brush = gcnew LinearGradientBrush(rect,
                Color::FromArgb(44, 62, 80), Color::FromArgb(52, 152, 219), 135.0f);
            e->Graphics->FillRectangle(brush, rect);
            delete brush;
        }

        // ============ ANIMATIONS ============

        void SetupAnimations() {
            timerFade = gcnew Timer();
            timerFade->Interval = 20;
            timerFade->Tick += gcnew EventHandler(this, &Login::timerFade_Tick);
            timerFade->Start();
        }

        void timerFade_Tick(Object^ sender, EventArgs^ e) {
            opacity += 0.05f;
            if (opacity >= 1.0f) {
                opacity = 1.0f;
                timerFade->Stop();
            }
            this->Opacity = opacity;
        }

        // ============ ACTIONS ============

        void llForgotPassword_LinkClicked(Object^ sender, LinkLabelLinkClickedEventArgs^ e) {
            MessageBox::Show(L"Contact administrator to reset password.\n\nEmail: support@carehub.com\nPhone: +20 123 456 789",
                L"Password Recovery",
                MessageBoxButtons::OK,
                MessageBoxIcon::Information);
        }

        void btnLogin_Click(Object^ sender, EventArgs^ e) {

            if (txtUsername->Text->Length == 0 || txtPassword->Text->Length == 0) {
                TriggerShake();

                MessageBox::Show(
                    L"Please enter email and password",
                    L"Login Required",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            DbConnection db;
            sql::Connection* conn = db.getConnect();

            if (conn == nullptr) {
                MessageBox::Show(
                    L"Database connection failed",
                    L"Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
                return;
            }

            try {

                using namespace System::Runtime::InteropServices;

                IntPtr emailPtr = Marshal::StringToHGlobalAnsi(txtUsername->Text);
                IntPtr passPtr = Marshal::StringToHGlobalAnsi(txtPassword->Text);

                String^ selectedRole =
                    cmbRole->SelectedItem->ToString()->Contains(L"Administrator")
                    ? L"Admin"
                    : L"Staff";

                IntPtr rolePtr = Marshal::StringToHGlobalAnsi(selectedRole);

                const char* email = static_cast<const char*>(emailPtr.ToPointer());
                const char* pass = static_cast<const char*>(passPtr.ToPointer());
                const char* role = static_cast<const char*>(rolePtr.ToPointer());

                sql::PreparedStatement* pstmt = conn->prepareStatement(
                    "SELECT email, role FROM users WHERE email=? AND password=? AND role=?"
                );

                pstmt->setString(1, email);
                pstmt->setString(2, pass);
                pstmt->setString(3, role);

                sql::ResultSet* res = pstmt->executeQuery();

                if (res->next()) {

                    UserRole = selectedRole;
                    Username = txtUsername->Text;

                    delete res;
                    delete pstmt;
                    delete conn;

                    Marshal::FreeHGlobal(emailPtr);
                    Marshal::FreeHGlobal(passPtr);
                    Marshal::FreeHGlobal(rolePtr);

                    this->DialogResult = System::Windows::Forms::DialogResult::OK;
                    this->Close();
                }
                else {

                    delete res;
                    delete pstmt;
                    delete conn;

                    Marshal::FreeHGlobal(emailPtr);
                    Marshal::FreeHGlobal(passPtr);
                    Marshal::FreeHGlobal(rolePtr);

                    TriggerShake();

                    MessageBox::Show(
                        L"Invalid Email, Password or Account Type",
                        L"Login Failed",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Error
                    );
                }
            }
            catch (sql::SQLException& ex) {

                TriggerShake();

                MessageBox::Show(
                    L"Database Error: " + gcnew String(ex.what()),
                    L"Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }
        void TriggerShake() {

            if (shakeTimer != nullptr) {
                shakeTimer->Stop();
                delete shakeTimer;
            }

            shakeOriginalLocation = panelForm->Location;
            shakeCount = 0;

            shakeTimer = gcnew Timer();
            shakeTimer->Interval = 40;
            shakeTimer->Tick += gcnew EventHandler(this, &Login::ShakeTimer_Tick);
            shakeTimer->Start();
        }
        void ShakeTimer_Tick(Object^ sender, EventArgs^ e) {
            if (shakeCount >= 6) {
                panelForm->Location = shakeOriginalLocation;
                shakeTimer->Stop();
                delete shakeTimer;
                shakeTimer = nullptr;
                shakeCount = 0;
            }
            else {
                int offset = (shakeCount % 2 == 0) ? -8 : 8;
                panelForm->Location = Point(shakeOriginalLocation.X + offset, shakeOriginalLocation.Y);
                shakeCount++;
            }
        }
    };
}
