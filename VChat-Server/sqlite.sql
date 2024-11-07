CREATE TABLE basic (
    id INT PRIMARY KEY,
    name_ TEXT,
    gender INT,
    age INT,
    signature TEXT
);

CREATE TABLE personal (
    id INT PRIMARY KEY,
    password_ INT, -- TODO: should be encryption!!!
    FOREIGN KEY (id) REFERENCES basic (id)
);

CREATE TABLE friend (
    id INT PRIMARY KEY,
    friendid INT,
    remark TEXT,
    FOREIGN KEY (id) REFERENCES basic (id)
);

CREATE TABLE message (
    id INT,
    sender INT,
    receiver INT,
    message TEXT,
    time_ INT
);
