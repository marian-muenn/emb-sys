#[macro_use]
extern crate rocket;
use rocket_db_pools::sqlx::types::chrono::*;
use rocket_db_pools::{sqlx, Database};
use serde_json::Value;

mod victron;

#[derive(Database)]
#[database("VictronPostgres")]
struct VictronPostgres(sqlx::PgPool);

#[get("/")]
fn index() -> &'static str {
    "_"
}

#[post("/victron_data", data = "<data>")]
async fn victron_data(pool: &VictronPostgres, data: String) -> &str {
    let v: Value = serde_json::from_str(data.as_str()).unwrap();
    println!("{:#?}", v);
    let payload: Value = serde_json::from_str(v["payload"].as_str().unwrap()).unwrap();
    let victron_frame = victron::VictronFrame::from_json_value(payload).unwrap();
    let timestamp = v["publish_received_at"].as_i64().unwrap();
    let station_id: i64 = v["topic"]
        .as_str()
        .unwrap()
        .split("/")
        .collect::<Vec<&str>>()[1]
        .parse::<i64>()
        .unwrap();
    let datetime: DateTime<Local> = Local.timestamp_millis_opt(timestamp).unwrap();
    println!("Received Frame: {:?} \n at {}", victron_frame, timestamp);
    sqlx::query(
        "INSERT INTO victron_measurement (
          time,
          station_id,
          pid,
          fw,
          serial_no,
          v_bat,
          i_bat,
          v_pv,
          p_pv,
          state,
          mppt_mode,
          error,
          load_active,
          i_load,
          yield_total,
          yield_today,
          max_power_today,
          yield_yesterday,
          max_power_yesterday,
          day_sequence_no
        ) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, $20);")
        .bind(datetime)
        .bind(station_id)
        .bind(victron_frame.pid)
        .bind(victron_frame.fw)
        .bind(victron_frame.serial_no)
        .bind(victron_frame.v_bat)
        .bind(victron_frame.i_bat)
        .bind(victron_frame.v_pv)
        .bind(victron_frame.p_pv)
        .bind(victron_frame.state)
        .bind(victron_frame.mppt_mode)
        .bind(victron_frame.error)
        .bind(victron_frame.load_active)
        .bind(victron_frame.i_load)
        .bind(victron_frame.yield_total)
        .bind(victron_frame.yield_today)
        .bind(victron_frame.max_power_today)
        .bind(victron_frame.yield_yesterday)
        .bind(victron_frame.max_power_yesterday)
        .bind(victron_frame.day_sequence_no)
        .execute(&**pool).await
        .and_then(|r|  Ok(r))
        .ok();
    return "inserted rows";
}

#[launch]
fn rocket() -> _ {
    rocket::build()
        .attach(VictronPostgres::init())
        .mount("/", routes![index, victron_data])
}
