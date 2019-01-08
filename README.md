# TrivialDB —— A Simple Database Engine

TrivialDB是一个简单的数据库管理系统，我们实现了大部分常见的SQL语句和类型。同时支持多表连接、复杂表达式运算、多主键约束、外键约束、CHECK约束、UNIQUE和DEFAULT约束、聚集查询、利用B+树索引的查询优化，同时，我们支持任意长度的VARCHAR类型。

## 编译及运行

你需要有支持C++11特性的编译器，以及Bison和Flex两个库。本项目通过CMake来构建，在根目录运行

```shell
ccmake .
```

进行编译选项的设置，之后运行

```shell
cmake .
make -j8
```

进行项目的编译，编译后的可执行程序在`build/trivial_db`目录下。

编译后可以选择在`testcase`目录下运行`python3 run_test.py`运行测试程序。

## 系统功能

### 数据类型

数据库支持的基本类型有：

 * 整型（INT）
 * 浮点型（FLOAT）
 * 字符串型（VARCHAR）
 * 日期型（DATE），日期格式`YYYY-MM-dd`

日期类型的字面值和字符串相同，在实现中如果必要可以转换为字符串。

### SQL语句

我们支持的SQL语句一共有如下几种

 * 插入语句：`INSERT INTO ... VALUES ...`
 * 删除语句：`DELETE FROM ... WHERE ...`
 * 查询语句：`SELECT ... FROM ... WHERE ...`
 * 更新语句：`UPDATE ... SET ... WHERE ...`
 * 创建数据库：`CREATE DATABASE ...`
 * 删除数据库：`DROP DATABASE ...`
 * 切换数据库：`USE ...`
 * 显示数据库信息：`SHOW DATABASE ...`
 * 创建表：`CREATE TABLE ...`
 * 删除表：`DROP TABLE ...`
 * 显示表信息：`SHOW TABLE ...`
 * 创建索引：`CREATE INDEX ...`
 * 删除索引：`DROP INDEX ...`

### 复杂表达式处理

表达式大致可以分为两种：算术表达式和条件表达式。由于采用Bison进行解析，可以支持任意深度嵌套的复杂表达式。我们所支持的基本运算主要如下

 * 四则运算，针对整数和浮点数进行。
 * 比较运算符，即<=, <, =, >, >=, <>。
 * 模糊匹配运算符，即LIKE，其实现采用C++11的正则表达式库。
 * 范围匹配运算符，即IN，可以在表的CHECK约束中以及WHERE子句中使用。
 * 空值判定运算符，即IS NULL和IS NOT NULL两种。
 * 逻辑运算，包含NOT、AND和OR三种。

以下是一些复杂表达式运算的例子

```sql
UPDATE customer SET age = age + 1 WHERE age < 18 AND gender = 'F';
SELECT * FROM customer WHERE name LIKE 'John %son';
SELECT * FROM students WHERE grades IN ('A', 'B', 'C');
SELECT * FROM students WHERE name IS NOT NULL;
```

### 聚集查询
我们实现了五种聚集查询函数COUNT、SUM、AVG、MIN和MAX。其中COUNT不支持DISTINCT关键字。例如

```sql
SELECT COUNT(*) FROM customer WHERE age > 18;
SELECT AVG(age) FROM customer WHERE age <= 18;
```
### 属性完整性约束
我们支持多种属性完整性约束，分别是

 * 主键约束。一个表可以有多个列联合起来作为主键，只有在所有主键都相同时才认为两条记录有冲突，即这种情况下主键是一个元组。
 * 外键约束，每个域都可以有外键约束，引用另外一个表的主键。
 * UNIQUE约束，该约束限制某一列的值不能重复。
 * NOT NULL约束，该约束限制某一列不能有空值。
 * DEFAULT约束，该约束可以在INSERT语句不指定值是给某列赋予一个默认值。
 * CHECK约束，该约束可以对表中元素的值添加条件表达式的检查。

下面是一个简单的例子，注意如果在多个列都指定了PRIMARY KEY，那么就认为主键是一个元组，而不是有多个主键。例如Infos表的主键为(PersonID, InfoID)。
```sql
CREATE TABLE Persons (
    PersonID int PRIMARY KEY NOT NULL,
    Name varchar(20),
    Age int DEFAULT 1,
    Gender varchar(1),
    CHECK (Age >= 1 AND Age <= 100),
    CHECK (Gender IN ('F', 'M'))
);

CREATE TABLE Infos (
    PersonID int PRIMARY KEY,
    InfoID int PRIMARY KEY,
    FOREIGN KEY (PersonID) REFERENCES Persons(PersonID)
);
```
### 多表连接查询
在SELECT语句中，我们支持任意多表的连接操作，例如
```sql
SELECT * FROM A, B, C WHERE A.ID = B.ID AND C.Name = A.Name
```
并且，对于多个表的连接中形如A.Col1 = B.Col2的条件，那么如果这两个列的某一个拥有索引，会利用索引进行查询优化。例如如下查询就可以优化

```sql
SELECT * FROM Persons, Infos WHERE Persons.PersonID = Infos.PersonID;
SELECT * FROM Persons, Infos, Datas WHERE Persons.PersonID = Infos.PersonID AND Datas.N IS NOT NULL;
SELECT * FROM Persons, Infos, Datas WHERE Persons.PersonID = Infos.PersonID AND Datas.ID = Infos.PersonID;
```
具体的优化方法以及何种查询可以优化见文档中"查询优化"部分。
### 表别名
我们在多表连接查询时支持通过别名（alias）的方式对一个表进行连接，例如
```sql
SELECT * FROM Persons AS P1, Persons AS P2 WHERE P1.PersonID = P2.PersonID;
```
## Acknowledgments

* 参考了往届的项目 https://github.com/Harry-Chen/SimpleDB
* SQL解析器部分参考了 https://github.com/thinkpad20/sql
