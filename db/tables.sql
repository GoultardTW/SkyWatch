DO $$
    BEGIN
        -- Verifica se il tipo ENUM 'status' esiste già
        IF NOT EXISTS (
            SELECT 1
            FROM pg_type
            WHERE typname = 'status'
              AND typtype = 'e'
        ) THEN
            -- Crea il tipo ENUM 'status' solo se non esiste già
            CREATE TYPE status AS ENUM ('READY', 'CHARGING', 'WORKING');
        END IF;

         -- Verifica se il tipo ENUM 'requirement' esiste già
        IF NOT EXISTS (
            SELECT 1
            FROM pg_type
            WHERE typname = 'requirement'
              AND typtype = 'e'
        ) THEN
            -- Crea il tipo ENUM 'requirement' solo se non esiste già
            CREATE TYPE requirement AS ENUM ('NUM_DRONES', 'CC_OVERLOAD', 'DRONE_AUTONOMY', 'AREA_COVERAGE', 'DRONE_OUT_OF_BATTERY');
        END IF;
END $$;

CREATE TABLE IF NOT EXISTS controlCenter (
    id SERIAL PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS drone (
    id SERIAL PRIMARY KEY,
    controlCenter int,
    batteryPercentage int CHECK (batteryPercentage >= 0 AND batteryPercentage <= 100),
    status status,
    FOREIGN KEY (controlCenter) REFERENCES controlCenter (id)
);

CREATE TABLE IF NOT EXISTS monitor_failure (
    failure_id SERIAL PRIMARY KEY,
    cc_id     int,
    failure   requirement,
    message   text,
    time      TIMESTAMP NOT NULL,
    FOREIGN KEY (cc_id) REFERENCES control_center (id)
);

CREATE TABLE IF NOT EXISTS session (
   id SERIAL PRIMARY KEY,
   controlCenter int,
   start_time TIMESTAMP NOT NULL,
   end_time   TIMESTAMP,
   FOREIGN KEY (controlCenter) REFERENCES controlCenter (id)
);