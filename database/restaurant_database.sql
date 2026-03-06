-- Create Database
CREATE DATABASE IF NOT EXISTS restaurant_management;
USE restaurant_management;

-- =========================
-- Staff Table
-- =========================
CREATE TABLE staff (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    cnic VARCHAR(15) NOT NULL UNIQUE,
    phone_no VARCHAR(15) NOT NULL,
    position VARCHAR(50) NOT NULL,
    salary DECIMAL(10,2) NOT NULL
);

-- =========================
-- Menu Table
-- =========================
CREATE TABLE menu (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    category VARCHAR(50) NOT NULL,
    cost_price DECIMAL(10,2) NOT NULL,
    selling_price DECIMAL(10,2) NOT NULL,
    image_path VARCHAR(255)
);

-- =========================
-- Restaurant Tables
-- =========================
CREATE TABLE restaurant_tables (
    table_no INT PRIMARY KEY
);

-- =========================
-- Order Counter
-- =========================
CREATE TABLE order_counter (
    id INT PRIMARY KEY,
    order_number INT NOT NULL
);

-- =========================
-- Orders Table
-- =========================
CREATE TABLE orders (
    order_no INT AUTO_INCREMENT PRIMARY KEY,
    total_price DECIMAL(10,2) NOT NULL,
    is_paid BOOLEAN DEFAULT 0,
    order_date DATE NOT NULL,
    table_no INT NULL,
    order_type VARCHAR(20) NOT NULL DEFAULT 'dine_in',
    payment_status VARCHAR(20) NOT NULL DEFAULT 'unpaid',
    status ENUM('Pending','In Progress','Ready','Paid','Cancelled') DEFAULT 'Pending',
    status_time DATETIME,
    notification_shown INT DEFAULT 0,
    FOREIGN KEY (table_no) REFERENCES restaurant_tables(table_no)
);

-- =========================
-- Order Items
-- =========================
CREATE TABLE order_items (
    item_id INT AUTO_INCREMENT PRIMARY KEY,
    order_no INT,
    item_name VARCHAR(100),
    quantity INT,
    unit_price DOUBLE,
    total_price DOUBLE,
    FOREIGN KEY (order_no) REFERENCES orders(order_no) ON DELETE CASCADE
);