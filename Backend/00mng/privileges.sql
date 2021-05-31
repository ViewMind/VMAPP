DROP USER IF EXISTS 'service_client_data_processing'@'%';
DROP USER IF EXISTS 'service_client_data_processing'@'localhost';

FLUSH PRIVILEGES;
CREATE USER 'service_client_data_processing'@'%' IDENTIFIED BY '1234';
DROP USER IF EXISTS 'service_administrative_management'@'%';
DROP USER IF EXISTS 'service_administrative_management'@'localhost';

FLUSH PRIVILEGES;
CREATE USER 'service_administrative_management'@'%' IDENTIFIED BY '1234';
DROP USER IF EXISTS 'service_client_data_processing'@'%';
DROP USER IF EXISTS 'service_client_data_processing'@'localhost';

FLUSH PRIVILEGES;
CREATE USER 'service_client_data_processing'@'%' IDENTIFIED BY '1234';
DROP USER IF EXISTS 'service_administrative_management'@'%';
DROP USER IF EXISTS 'service_administrative_management'@'localhost';

FLUSH PRIVILEGES;
CREATE USER 'service_administrative_management'@'%' IDENTIFIED BY '1234';
GRANT SELECT ON vm_main.institution TO 'service_client_data_processing'@'%';
GRANT SELECT ON vm_main.institution_users TO 'service_client_data_processing'@'%';
GRANT SELECT ON vm_main.portal_users TO 'service_client_data_processing'@'%';
GRANT SELECT,INSERT,UPDATE ON vm_main.institution TO 'service_administrative_management'@'%';
GRANT SELECT,INSERT,UPDATE ON vm_main.institution_users TO 'service_administrative_management'@'%';
GRANT SELECT,INSERT,UPDATE ON vm_main.portal_users TO 'service_administrative_management'@'%';
GRANT SELECT ON vm_secure.placed_products TO 'service_client_data_processing'@'%';
GRANT SELECT ON vm_secure.secrets TO 'service_client_data_processing'@'%';
GRANT SELECT,INSERT,UPDATE ON vm_secure.placed_products TO 'service_administrative_management'@'%';
GRANT SELECT,INSERT,UPDATE ON vm_secure.secrets TO 'service_administrative_management'@'%';
