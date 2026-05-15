#pragma once
#include "DbConnection.h"

namespace HospitalManagementSystem {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Drawing::Drawing2D;
    using namespace System::Collections::Generic;

    public ref class Pharmacy : public Form {
    private:
        Panel^ headerPanel; Panel^ sidebarPanel; Panel^ contentPanel; Panel^ statsPanel; Panel^ mainContentPanel;
        DataGridView^ dgvMedicines; TextBox^ txtSearch;
        TextBox^ txtName; TextBox^ txtQuantity; TextBox^ txtPrice; TextBox^ txtExpiry;
        Button^ btnAdd; Button^ btnUpdate; Button^ btnDelete; Button^ btnClear;
        Label^ lblDateTime; Label^ lblWelcome; PictureBox^ pbAvatar;
        Panel^ notificationPanel; Label^ lblNotificationCount;
        Timer^ timerDateTime; List<Dictionary<String^, String^>^>^ medicinesList;
        int nextId; String^ userName;

    public:
        Pharmacy() {
            medicinesList = gcnew List<Dictionary<String^, String^>^>();
            nextId = 5; userName = L"Admin";
            InitializeComponent(); LoadSampleData(); LoadStyles();
            RefreshGrid(); UpdateDateTime(); LoadStatsCards();
        }

    protected:
        ~Pharmacy() {
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
            g->FillEllipse(gcnew SolidBrush(Color::FromArgb(241, 196, 15)), pic->ClientRectangle);
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

        void SetPlaceholder(TextBox^ txt, String^ placeholder) {
            if (txt == nullptr) return;
            // Placeholder functionality - simplified without DllImport
            if (txt->Text->Length == 0) {
                txt->Text = placeholder;
                txt->ForeColor = Color::FromArgb(150, 150, 150);
            }
        }

        void InitializeComponent() {
            this->Text = L"CareHub - Pharmacy Management";
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
            lblPageTitle->Text = L"💊 Pharmacy Management";
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
            notificationPanel->Paint += gcnew PaintEventHandler(this, &Pharmacy::NotificationPanel_Paint);

            pbAvatar = gcnew PictureBox();
            pbAvatar->Size = System::Drawing::Size(48, 48);
            pbAvatar->BackColor = Color::FromArgb(241, 196, 15);
            pbAvatar->Anchor = AnchorStyles::Top | AnchorStyles::Right;
            pbAvatar->Paint += gcnew PaintEventHandler(this, &Pharmacy::PbAvatar_Paint);

            headerPanel->Controls->Add(lblPageTitle);
            headerPanel->Controls->Add(lblWelcome);
            headerPanel->Controls->Add(lblDateTime);
            headerPanel->Controls->Add(notificationPanel);
            headerPanel->Controls->Add(pbAvatar);

            // ============ SIDEBAR ============
            sidebarPanel = gcnew Panel();
            sidebarPanel->Dock = DockStyle::Left;
            sidebarPanel->Width = 250;
            sidebarPanel->BackColor = Color::FromArgb(44, 62, 80);
            sidebarPanel->Padding = System::Windows::Forms::Padding(0, 70, 0, 0);

            Label^ lblSidebarTitle = gcnew Label();
            lblSidebarTitle->Text = L"📋 Menu";
            lblSidebarTitle->Font = gcnew System::Drawing::Font(L"Segoe UI", 13, FontStyle::Bold);
            lblSidebarTitle->ForeColor = Color::White;
            lblSidebarTitle->Location = Point(15, 20);
            lblSidebarTitle->Size = System::Drawing::Size(150, 35);

            sidebarPanel->Controls->Add(lblSidebarTitle);

            // ============ CONTENT PANEL ============
            contentPanel = gcnew Panel();
            contentPanel->Dock = DockStyle::Fill;
            contentPanel->BackColor = Color::FromArgb(240, 242, 245);
            contentPanel->Padding = System::Windows::Forms::Padding(15, 15, 25, 20);
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
            txtSearch->Text = L"Search by medicine name...";
            txtSearch->ForeColor = Color::FromArgb(150, 150, 150);
            txtSearch->Enter += gcnew EventHandler(this, &Pharmacy::txtSearch_Enter);
            txtSearch->Leave += gcnew EventHandler(this, &Pharmacy::txtSearch_Leave);
            txtSearch->TextChanged += gcnew EventHandler(this, &Pharmacy::txtSearch_TextChanged);

            searchContainer->Controls->Add(lblSearchIcon);
            searchContainer->Controls->Add(txtSearch);

            Button^ btnAddNew = gcnew Button();
            btnAddNew->Text = L"+ Add Medicine";
            btnAddNew->Location = Point(440, 8);
            btnAddNew->Size = System::Drawing::Size(170, 45);
            btnAddNew->BackColor = Color::FromArgb(241, 196, 15);
            btnAddNew->ForeColor = Color::FromArgb(44, 62, 80);
            btnAddNew->FlatStyle = FlatStyle::Flat;
            btnAddNew->Font = gcnew System::Drawing::Font(L"Segoe UI", 11, FontStyle::Bold);
            btnAddNew->Cursor = Cursors::Hand;
            btnAddNew->Click += gcnew EventHandler(this, &Pharmacy::ScrollToInput);

            topBar->Controls->Add(searchContainer);
            topBar->Controls->Add(btnAddNew);

            // DataGridView
            dgvMedicines = gcnew DataGridView();
            dgvMedicines->Dock = DockStyle::Fill;
            dgvMedicines->BackgroundColor = Color::White;
            dgvMedicines->BorderStyle = BorderStyle::None;
            dgvMedicines->RowHeadersVisible = false;
            dgvMedicines->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
            dgvMedicines->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
            dgvMedicines->AllowUserToAddRows = false;
            dgvMedicines->RowTemplate->Height = 38;
            dgvMedicines->CellClick += gcnew DataGridViewCellEventHandler(this, &Pharmacy::dgvMedicines_CellClick);

            // Bottom Input Panel
            Panel^ inputPanel = gcnew Panel();
            inputPanel->Dock = DockStyle::Bottom;
            inputPanel->Height = 200;
            inputPanel->BackColor = Color::FromArgb(248, 249, 250);
            inputPanel->BorderStyle = BorderStyle::FixedSingle;
            inputPanel->Padding = System::Windows::Forms::Padding(15, 10, 15, 10);

            Label^ lblFormTitle = gcnew Label();
            lblFormTitle->Text = L"📝 Medicine Information";
            lblFormTitle->Font = gcnew System::Drawing::Font(L"Segoe UI", 13, FontStyle::Bold);
            lblFormTitle->ForeColor = Color::FromArgb(44, 62, 80);
            lblFormTitle->Location = Point(5, 8);
            lblFormTitle->Size = System::Drawing::Size(250, 30);

            Label^ lblName = gcnew Label();
            lblName->Text = L"Medicine Name:*";
            lblName->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblName->Location = Point(15, 55);
            lblName->Size = System::Drawing::Size(110, 25);

            txtName = gcnew TextBox();
            txtName->Location = Point(130, 52);
            txtName->Size = System::Drawing::Size(230, 35);
            txtName->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);

            Label^ lblQuantity = gcnew Label();
            lblQuantity->Text = L"Quantity:*";
            lblQuantity->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblQuantity->Location = Point(380, 55);
            lblQuantity->Size = System::Drawing::Size(80, 25);

            txtQuantity = gcnew TextBox();
            txtQuantity->Location = Point(465, 52);
            txtQuantity->Size = System::Drawing::Size(100, 35);
            txtQuantity->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);

            Label^ lblPrice = gcnew Label();
            lblPrice->Text = L"Price (EGP):*";
            lblPrice->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblPrice->Location = Point(585, 55);
            lblPrice->Size = System::Drawing::Size(90, 25);

            txtPrice = gcnew TextBox();
            txtPrice->Location = Point(680, 52);
            txtPrice->Size = System::Drawing::Size(120, 35);
            txtPrice->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);

            Label^ lblExpiry = gcnew Label();
            lblExpiry->Text = L"Expiry Date:";
            lblExpiry->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
            lblExpiry->Location = Point(15, 105);
            lblExpiry->Size = System::Drawing::Size(90, 25);

            txtExpiry = gcnew TextBox();
            txtExpiry->Location = Point(110, 102);
            txtExpiry->Size = System::Drawing::Size(150, 35);
            txtExpiry->Font = gcnew System::Drawing::Font(L"Segoe UI", 11);
            txtExpiry->Text = L"YYYY-MM-DD";
            txtExpiry->ForeColor = Color::FromArgb(150, 150, 150);
            txtExpiry->Enter += gcnew EventHandler(this, &Pharmacy::txtExpiry_Enter);
            txtExpiry->Leave += gcnew EventHandler(this, &Pharmacy::txtExpiry_Leave);

            btnAdd = CreateButton(L"➕ Add", Color::FromArgb(46, 204, 113), 15, 155, 110, 42);
            btnUpdate = CreateButton(L"✏️ Update", Color::FromArgb(52, 152, 219), 140, 155, 110, 42);
            btnDelete = CreateButton(L"🗑️ Delete", Color::FromArgb(231, 76, 60), 265, 155, 110, 42);
            btnClear = CreateButton(L"🔄 Clear", Color::FromArgb(149, 165, 166), 390, 155, 110, 42);

            btnAdd->Click += gcnew EventHandler(this, &Pharmacy::btnAdd_Click);
            btnUpdate->Click += gcnew EventHandler(this, &Pharmacy::btnUpdate_Click);
            btnDelete->Click += gcnew EventHandler(this, &Pharmacy::btnDelete_Click);
            btnClear->Click += gcnew EventHandler(this, &Pharmacy::btnClear_Click);

            inputPanel->Controls->Add(lblFormTitle);
            inputPanel->Controls->Add(lblName); inputPanel->Controls->Add(txtName);
            inputPanel->Controls->Add(lblQuantity); inputPanel->Controls->Add(txtQuantity);
            inputPanel->Controls->Add(lblPrice); inputPanel->Controls->Add(txtPrice);
            inputPanel->Controls->Add(lblExpiry); inputPanel->Controls->Add(txtExpiry);
            inputPanel->Controls->Add(btnAdd); inputPanel->Controls->Add(btnUpdate);
            inputPanel->Controls->Add(btnDelete); inputPanel->Controls->Add(btnClear);

            mainContentPanel->Controls->Add(topBar);
            mainContentPanel->Controls->Add(dgvMedicines);
            mainContentPanel->Controls->Add(inputPanel);

            contentPanel->Controls->Add(statsPanel);
            contentPanel->Controls->Add(mainContentPanel);

            this->Controls->Add(contentPanel);
            this->Controls->Add(sidebarPanel);
            this->Controls->Add(headerPanel);

            timerDateTime = gcnew Timer();
            timerDateTime->Interval = 1000;
            timerDateTime->Tick += gcnew EventHandler(this, &Pharmacy::timerDateTime_Tick);
            timerDateTime->Start();

            this->Resize += gcnew EventHandler(this, &Pharmacy::OnResize);
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

            int totalQty = 0; int lowStock = 0;
            for each (Dictionary<String^, String^> ^ med in medicinesList) {
                int qty = Int32::Parse(med[L"Quantity"]);
                totalQty += qty;
                if (qty < 50) lowStock++;
            }

            String^ title1 = L"Total Medicines";
            String^ title2 = L"Total Stock";
            String^ title3 = L"Low Stock (<50)";
            String^ title4 = L"Expiring Soon";

            String^ value1 = medicinesList->Count.ToString();
            String^ value2 = totalQty.ToString();
            String^ value3 = lowStock.ToString();
            String^ value4 = L"2";

            String^ icon1 = L"💊";
            String^ icon2 = L"📦";
            String^ icon3 = L"⚠️";
            String^ icon4 = L"⏰";

            Color color1 = Color::FromArgb(241, 196, 15);
            Color color2 = Color::FromArgb(52, 152, 219);
            Color color3 = Color::FromArgb(231, 76, 60);
            Color color4 = Color::FromArgb(155, 89, 182);

            int cardWidth = 245;
            int startX = 5;

            cli::array<String^>^ titles = { title1, title2, title3, title4 };
            cli::array<String^>^ values = { value1, value2, value3, value4 };
            cli::array<String^>^ icons = { icon1, icon2, icon3, icon4 };
            cli::array<Color>^ colors = { color1, color2, color3, color4 };

            for (int i = 0; i < 4; i++) {
                Panel^ card = gcnew Panel();
                card->Size = System::Drawing::Size(cardWidth, 115);
                card->Location = Point(startX + (i * (cardWidth + 12)), 8);
                card->BackColor = Color::White;

                Panel^ iconCircle = gcnew Panel();
                iconCircle->Size = System::Drawing::Size(48, 48);
                iconCircle->Location = Point(15, 15);
                iconCircle->BackColor = Color::FromArgb(253, 245, 210);

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
            AddMedicine(L"Paracetamol 500mg", L"500", L"25.00", L"2025-12-31");
            AddMedicine(L"Amoxicillin 250mg", L"300", L"45.00", L"2025-10-15");
            AddMedicine(L"Vitamin C 1000mg", L"200", L"35.00", L"2026-01-20");
            AddMedicine(L"Ibuprofen 400mg", L"400", L"30.00", L"2025-09-10");
        }

        void AddMedicine(String^ name, String^ quantity, String^ price, String^ expiry) {
            Dictionary<String^, String^>^ med = gcnew Dictionary<String^, String^>();
            med->Add(L"ID", (nextId++).ToString()); med->Add(L"Name", name);
            med->Add(L"Quantity", quantity); med->Add(L"Price", price);
            med->Add(L"Expiry", expiry); medicinesList->Add(med);
        }

        void RefreshGrid() {
            dgvMedicines->Columns->Clear();
            dgvMedicines->Rows->Clear();
            dgvMedicines->Columns->Add(L"ID", L"ID");
            dgvMedicines->Columns->Add(L"Name", L"Medicine Name");
            dgvMedicines->Columns->Add(L"Quantity", L"Quantity");
            dgvMedicines->Columns->Add(L"Price", L"Price (EGP)");
            dgvMedicines->Columns->Add(L"Expiry", L"Expiry Date");

            for each (Dictionary<String^, String^> ^ med in medicinesList) {
                dgvMedicines->Rows->Add(med[L"ID"], med[L"Name"], med[L"Quantity"], med[L"Price"], med[L"Expiry"]);
            }
            LoadStatsCards();
        }

        void SearchMedicines(String^ searchTerm) {
            dgvMedicines->Rows->Clear();
            for each (Dictionary<String^, String^> ^ med in medicinesList) {
                if (med[L"Name"]->ToLower()->Contains(searchTerm->ToLower())) {
                    dgvMedicines->Rows->Add(med[L"ID"], med[L"Name"], med[L"Quantity"], med[L"Price"], med[L"Expiry"]);
                }
            }
        }

        void LoadStyles() {
            dgvMedicines->EnableHeadersVisualStyles = false;
            dgvMedicines->ColumnHeadersDefaultCellStyle->BackColor = Color::FromArgb(241, 196, 15);
            dgvMedicines->ColumnHeadersDefaultCellStyle->ForeColor = Color::FromArgb(44, 62, 80);
            dgvMedicines->ColumnHeadersDefaultCellStyle->Font = gcnew System::Drawing::Font(L"Segoe UI", 11, FontStyle::Bold);
            dgvMedicines->AlternatingRowsDefaultCellStyle->BackColor = Color::FromArgb(248, 249, 250);
            ApplyRoundedRegion(mainContentPanel, 12);
        }

        void OnResize(Object^ sender, EventArgs^ e) {
            lblDateTime->Location = Point(this->ClientSize.Width - 340, 22);
            notificationPanel->Location = Point(this->ClientSize.Width - 170, 14);
            pbAvatar->Location = Point(this->ClientSize.Width - 100, 11);
        }

        void timerDateTime_Tick(Object^ sender, EventArgs^ e) {
            UpdateDateTime();
        }

        void UpdateDateTime() {
            lblDateTime->Text = DateTime::Now.ToString(L"dddd, MMMM dd, yyyy  hh:mm:ss tt");
        }

        void ScrollToInput(Object^ sender, EventArgs^ e) {
            txtName->Focus();
        }

        void txtSearch_Enter(Object^ sender, EventArgs^ e) {
            if (txtSearch->Text == L"Search by medicine name...") {
                txtSearch->Text = L"";
                txtSearch->ForeColor = Color::Black;
            }
        }

        void txtSearch_Leave(Object^ sender, EventArgs^ e) {
            if (txtSearch->Text->Length == 0) {
                txtSearch->Text = L"Search by medicine name...";
                txtSearch->ForeColor = Color::FromArgb(150, 150, 150);
            }
        }

        void txtSearch_TextChanged(Object^ sender, EventArgs^ e) {
            if (txtSearch->Text != L"Search by medicine name..." && txtSearch->Text->Length > 0)
                SearchMedicines(txtSearch->Text);
            else
                RefreshGrid();
        }

        void dgvMedicines_CellClick(Object^ sender, DataGridViewCellEventArgs^ e) {
            if (e->RowIndex >= 0) {
                DataGridViewRow^ row = dgvMedicines->Rows[e->RowIndex];
                txtName->Text = row->Cells[1]->Value->ToString();
                txtQuantity->Text = row->Cells[2]->Value->ToString();
                txtPrice->Text = row->Cells[3]->Value->ToString();
                txtExpiry->Text = row->Cells[4]->Value->ToString();
            }
        }

        void btnAdd_Click(Object^ sender, EventArgs^ e) {
            if (txtName->Text->Length > 0 && txtQuantity->Text->Length > 0 && txtPrice->Text->Length > 0) {
                AddMedicine(txtName->Text, txtQuantity->Text, txtPrice->Text, txtExpiry->Text);
                RefreshGrid();
                ClearFields();
                MessageBox::Show(L"✓ Medicine added!", L"Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            else
                MessageBox::Show(L"Please fill required fields", L"Error", MessageBoxButtons::OK, MessageBoxIcon::Warning);
        }

        void btnUpdate_Click(Object^ sender, EventArgs^ e) {
            if (dgvMedicines->SelectedRows->Count > 0) {

                int index = dgvMedicines->SelectedRows[0]->Index;

                if (index < medicinesList->Count) {

                    medicinesList[index]->default[L"Name"] = txtName->Text;
                    medicinesList[index]->default[L"Quantity"] = txtQuantity->Text;
                    medicinesList[index]->default[L"Price"] = txtPrice->Text;
                    medicinesList[index]->default[L"Expiry"] = txtExpiry->Text;

                    RefreshGrid();
                    ClearFields();

                    MessageBox::Show(
                        L"✓ Medicine updated!",
                        L"Success",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Information
                    );
                }
            }
            else {
                MessageBox::Show(
                    L"Please select a medicine to update",
                    L"Info",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
            }
        }

        void btnDelete_Click(Object^ sender, EventArgs^ e) {
            if (dgvMedicines->SelectedRows->Count > 0) {
                if (MessageBox::Show(L"Delete this medicine?", L"Confirm", MessageBoxButtons::YesNo, MessageBoxIcon::Question)
                    == System::Windows::Forms::DialogResult::Yes) {
                    medicinesList->RemoveAt(dgvMedicines->SelectedRows[0]->Index);
                    RefreshGrid();
                    ClearFields();
                }
            }
            else
                MessageBox::Show(L"Please select a medicine to delete", L"Info", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }

        void btnClear_Click(Object^ sender, EventArgs^ e) {
            ClearFields();
        }

        void ClearFields() {
            txtName->Clear();
            txtQuantity->Clear();
            txtPrice->Clear();
            txtExpiry->Clear();
        }

        void txtExpiry_Enter(Object^ sender, EventArgs^ e) {
            if (txtExpiry->Text == L"YYYY-MM-DD") {
                txtExpiry->Text = L"";
                txtExpiry->ForeColor = Color::Black;
            }
        }

        void txtExpiry_Leave(Object^ sender, EventArgs^ e) {
            if (txtExpiry->Text->Length == 0) {
                txtExpiry->Text = L"YYYY-MM-DD";
                txtExpiry->ForeColor = Color::FromArgb(150, 150, 150);
            }
        }
    };
}
