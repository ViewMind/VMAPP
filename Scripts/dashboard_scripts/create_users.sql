
-- Details: viewmind_dashboard_users, dashboard_user with 2019vmDashaboard

DROP TABLE IF EXISTS tUsers;

CREATE TABLE tUsers (
   name       VARCHAR(255),
   lastname   VARCHAR(255),
   passwd     VARCHAR(255),
   email      VARCHAR(255),
   permisions INT,
   keyid      PRIMARY KEY AUTO_INCREMENT
);


