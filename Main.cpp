// Main.cpp
#include "Login.h"
#include "Dashboard.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main(cli::array<String^>^ args) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    HospitalManagementSystem::Login^ loginForm = gcnew HospitalManagementSystem::Login();

    if (loginForm->ShowDialog() == DialogResult::OK) {
        HospitalManagementSystem::Dashboard^ dashboard =
            gcnew HospitalManagementSystem::Dashboard(loginForm->UserRole, loginForm->Username);
        Application::Run(dashboard);
    }

    return 0;
}
