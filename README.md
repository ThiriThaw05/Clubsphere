# 📚 **ClubSphere**

A **centralized student club management platform** designed to enhance communication, event organization, and engagement within university clubs.

Built using **C++ (Qt Framework)** and **SQLite**, ClubSphere streamlines club activities for students, leaders, and administrators through a **gamified, role-based management system**.

---

## 📌 **Overview**

University students often face challenges in discovering clubs, joining processes, and staying updated on events. Meanwhile, club leaders struggle with managing members and events efficiently. **ClubSphere addresses these challenges by providing a centralized, user-friendly platform with real-time updates, role-specific dashboards, and leaderboards to foster a dynamic student community.**

---

## 🎨 **Key Features**

### 👩‍💻 **Admin Dashboard**
- **Manage registered users, clubs, and events**
- **View leaderboards for top clubs and users**
- **Approve or remove club data**

### 👑 **Leader Dashboard**
- **Create and manage club-specific events**
- **Track attendance through event codes**
- **In-app group chat and notification system**
- **Leaderboards for club-specific and university-wide rankings**

### 🧑‍🎓 **Member Experience**
- **Explore, join, and participate in clubs**
- **Earn points for event participation**
- **Access personal profiles with points, ranks, and club activity**
- **View dynamic leaderboards and receive club notifications**

### 🎮 **Gamification & Ranking System**
- **Points-based system to reward event participation**
- **Leaderboards ranking members, clubs, and leaders to encourage healthy competition**

---

## ⚙️ **Tech Stack**

- **C++**
- **Qt Framework (GUI)**
- **SQLite (Database)**

---

## 📥 **Installation & How to Run**

### 🔧 **Prerequisites**
- **C++ compiler** (GCC, MinGW, or Visual Studio)
- **Qt Creator** (recommended) or Qt libraries installed on your system

### 📦 **Setup Instructions**

1. **Clone the Repository**
   ```bash
   git clone https://github.com/yourusername/Clubsphere.git
2. **Open Project**
  - Launch Qt Creator
  - Open the .pro project file from the cloned directory.
3. **Configure Build Settings**
  - Set your build directory.
  - Ensure the Qt version is properly selected.
4. **Build and Run**
  - Click the Build button (hammer icon)
  - Run the application from within Qt Creator.
5. **Database Initialization**
  - The SQLite database file will be auto-created upon first launch
  - Default admin and sample data can be added via the admin panel
___
### 📂 **Project Structure**
```bash
Clubsphere/
├── assets/               # Profile pictures and images
├── database/             # SQLite database files
├── src/                  # C++ source files
├── ui/                   # Qt Designer .ui files
├── main.cpp              # Application entry point
├── Clubsphere.pro        # Qt project file
└── README.md
