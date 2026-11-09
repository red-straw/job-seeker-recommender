USE database_theory;

DROP PROCEDURE IF EXISTS insert_random_job_seekers;

DELIMITER $$

CREATE PROCEDURE insert_random_job_seekers()
BEGIN
  DECLARE i INT DEFAULT 0;
  WHILE i < 100 DO
    INSERT INTO job_seekers (name, experience_years, certifications, education_level, technical_score)
    VALUES (
      CONCAT('지원자', i + 1),
      FLOOR(RAND() * 11),
      FLOOR(RAND() * 7),
      FLOOR(RAND() * 4) + 1,
      FLOOR(RAND() * 51) + 50
    );
    SET i = i + 1;
  END WHILE;
END$$

DELIMITER ;

CALL insert_random_job_seekers();
