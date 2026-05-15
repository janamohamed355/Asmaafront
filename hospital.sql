create database if not exists hospital ;
use hospital ;
CREATE TABLE  if not exists Patients  (
    Patient_ID INT PRIMARY KEY AUTO_INCREMENT,
    Name VARCHAR(50) NOT NULL,
    Age INT,
    Gender VARCHAR(10),
    Phone VARCHAR(15),
    Address TEXT
);
CREATE TABLE  if not exists Doctors (
    Doctor_ID INT PRIMARY KEY AUTO_INCREMENT,
    Name VARCHAR(50) NOT NULL,
    Specialization VARCHAR(50),
    Phone VARCHAR(15)
);
CREATE TABLE IF NOT EXISTS Appointments (
    Appointment_ID INT PRIMARY KEY AUTO_INCREMENT,
    Patient_ID INT,
    Doctor_ID INT,
    Appointment_Date DATE,
    Appointment_Time TIME,
    FOREIGN KEY (Patient_ID) REFERENCES Patients(Patient_ID),
    FOREIGN KEY (Doctor_ID) REFERENCES Doctors(Doctor_ID)
);
CREATE TABLE IF NOT EXISTS Rooms (
    Room_ID INT PRIMARY KEY AUTO_INCREMENT,
    Room_Type VARCHAR(20),
    Status VARCHAR(20),
    Price VARCHAR(20)
);
CREATE TABLE  if not exists Admissions (
    Admission_ID INT PRIMARY KEY AUTO_INCREMENT,
    Patient_ID INT,
    Room_ID INT,
    Admit_Date DATE,
    Discharge_Date DATE,
    FOREIGN KEY (Patient_ID) REFERENCES Patients(Patient_ID),
    FOREIGN KEY (Room_ID) REFERENCES Rooms(Room_ID)
);
CREATE TABLE  if not exists Users (
    User_ID INT PRIMARY KEY AUTO_INCREMENT,
    Username VARCHAR(50) UNIQUE,
    Password VARCHAR(50),
    Role VARCHAR(20) -- Admin / Doctor / Receptionist
);
INSERT INTO Patients (Name, Age, Gender, Phone, Address)
VALUES 
('Ahmed Ali', 25, 'Male', '01012345678', 'Alexandria'),
('Sara Mohamed', 30, 'Female', '01198765432', 'Cairo');
INSERT INTO Doctors (Name, Specialization, Phone)
VALUES 
('Dr Omar', 'Cardiology', '01234567890'),
('Dr Mona', 'Dermatology', '01011223344');
INSERT INTO Appointments (Patient_ID, Doctor_ID, Appointment_Date, Appointment_Time)
VALUES 
(1, 1, '2026-04-24', '10:00:00'),
(2, 2, '2026-04-24', '12:00:00');
INSERT INTO Rooms (Room_Type, Status, Price)
VALUES 
('ICU', 'Occupied', '3000'),
('Normal', 'Available', '500'),
('Private Room', 'Available', '1500'),
('VIP Suite', 'Occupied', '5000');
INSERT INTO Admissions (Patient_ID, Room_ID, Admit_Date, Discharge_Date)
VALUES 
(1, 1, '2026-04-20', NULL),
(2, 2, '2026-04-21', '2026-04-23');
INSERT INTO Users (Username, Password, Role)
VALUES 
('admin', '1234', 'Admin'),
('dr_omar', 'dr123', 'Doctor'),
('reception1', 'rec123', 'Receptionist')
