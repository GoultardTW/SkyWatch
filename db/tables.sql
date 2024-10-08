DO $$
    BEGIN
        IF NOT EXISTS (
            SELECT 1
            FROM pg_type
            WHERE typname = 'requirement'
              AND typtype = 'e'
        ) THEN
            CREATE TYPE requirement AS ENUM ('MISSING_REPORT', 'PATH_CALCULATION', 'AREA_COVERAGE', 'NUM_DRONES', 'CC_OVERLOAD');
        END IF;
END $$;

CREATE TABLE IF NOT EXISTS controlCenter (
    id SERIAL PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS drone (
    id SERIAL PRIMARY KEY,
    id_cdc INT,
    battery int CHECK (battery >= 0 AND battery <= 100),
    FOREIGN KEY (id_cdc) REFERENCES controlCenter (id)
);

CREATE TABLE IF NOT EXISTS session_ (
   id SERIAL PRIMARY KEY,
   id_cdc INT,
   start_ TIMESTAMP NOT NULL,
   end_   TIMESTAMP,
   FOREIGN KEY (id_cdc) REFERENCES controlCenter (id)
);

CREATE TABLE IF NOT EXISTS monitor_failure (
    id SERIAL PRIMARY KEY,
    session_ INT,
    failure requirement,
    message_ TEXT,
    date_time TIMESTAMP NOT NULL,
    FOREIGN KEY (session_) REFERENCES session_ (id)
)