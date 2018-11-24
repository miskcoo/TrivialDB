CREATE DATABASE db;
USE DATABASE db;
CREATE TABLE Persons (
    PersonID int,
    LastName varchar(255),
    FirstName varchar(255),
    Address varchar(255),
    City varchar(255)
);
INSERT INTO Persons VALUES 
	(23, 'Yi', 'Wang', 'Tsinghua Univ.', 'Beijing'),
	(-238, 'Zhong', 'Lei', 'Beijing Univ.', 'Neijing'),
	(1+999, 'Wasserstein', 'Zhang', 'Hunan Univ.', 'Hunan');
EXIT;
