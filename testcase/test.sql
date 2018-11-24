CREATE DATABASE db;
USE db;
CREATE TABLE Persons (
    PersonID int,
    LastName varchar(20),
    FirstName varchar(20),
    Address varchar(20),
    City varchar(10)
);
CREATE INDEX Persons(PersonID);
CREATE INDEX Persons(FirstName);
INSERT INTO Persons VALUES 
	(23, 'Yi', 'Wang', 'Tsinghua Univ.', 'Beijing'),
	(-238, 'Zhong', 'Lei', 'Beijing Univ.', 'Neijing'),
	(1+999, 'Wasserstein', 'Zhang', 'Hunan Univ.', 'Hunan');
EXIT;
