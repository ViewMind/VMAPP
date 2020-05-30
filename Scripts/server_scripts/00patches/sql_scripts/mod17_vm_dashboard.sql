
ALTER TABLE tProduction ADD COLUMN vm_soft_install TINYINT;
ALTER TABLE tProduction ADD COLUMN vm_soft_install_rejection INT;

ALTER TABLE tProduction ADD COLUMN vm_soft_install_mod TIMESTAMP;
ALTER TABLE tProduction ADD COLUMN vm_soft_install_user VARCHAR(255);

ALTER TABLE tProduction ADD COLUMN pc_configuration_mod TIMESTAMP;
ALTER TABLE tProduction ADD COLUMN pc_configuration_user VARCHAR(255);

ALTER TABLE tProduction ADD COLUMN et_install_mod  TIMESTAMP;
ALTER TABLE tProduction ADD COLUMN et_install_user VARCHAR(255);

ALTER TABLE tProduction ADD COLUMN eyeexp_install_mod TIMESTAMP;
ALTER TABLE tProduction ADD COLUMN eyeexp_install_user VARCHAR(255);

ALTER TABLE tProduction ADD COLUMN final_control_mod TIMESTAMP;
ALTER TABLE tProduction ADD COLUMN final_control_user VARCHAR(255);

ALTER TABLE tProduction ADD COLUMN labelling_mod TIMESTAMP;
ALTER TABLE tProduction ADD COLUMN labelling_user VARCHAR(255);

ALTER TABLE tProduction ADD COLUMN packaging_mod TIMESTAMP;
ALTER TABLE tProduction ADD COLUMN packaging_user VARCHAR(255);

ALTER TABLE tProduction ADD COLUMN final_release_mod  TIMESTAMP;
ALTER TABLE tProduction ADD COLUMN final_release_user VARCHAR(255);

ALTER TABLE tProduction DROP COLUMN eyeexp_install;
ALTER TABLE tProduction DROP COLUMN eyeexp_install_rejection;

UPDATE tProduction SET vm_soft_install = 1;

UPDATE tProduction SET vm_soft_install_mod = mod_last;
UPDATE tProduction SET pc_configuration_mod = mod_last;
UPDATE tProduction SET et_install_mod = mod_last;
UPDATE tProduction SET eyeexp_install_mod = mod_last;
UPDATE tProduction SET final_control_mod = mod_last;
UPDATE tProduction SET packaging_mod = mod_last;
UPDATE tProduction SET labelling_mod = mod_last;
UPDATE tProduction SET final_release_mod = mod_last;

UPDATE tProduction SET vm_soft_install_user = mod_user;
UPDATE tProduction SET pc_configuration_user = mod_user;
UPDATE tProduction SET et_install_user = mod_user;
UPDATE tProduction SET eyeexp_install_user = mod_user;
UPDATE tProduction SET final_control_user = mod_user;
UPDATE tProduction SET labelling_user = mod_user;
UPDATE tProduction SET packaging_user = mod_user;
UPDATE tProduction SET final_release_user = mod_user;

