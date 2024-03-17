use serde_json::{Value, Result};
#[derive(Debug)]
pub struct VictronFrame {
	pub pid: String,
	pub fw: i64,
	pub serial_no: String,
	pub v_bat: i64,
	pub i_bat: i64,
	pub v_pv: i64,
	pub p_pv: i64,
	pub state: i64,
	pub mppt_mode: i64,
	pub error: i64,
	pub load_active: bool,
	pub i_load: i64,
	pub yield_total: i64,
    pub yield_today:  i64,
	pub max_power_today: i64,
	pub yield_yesterday: i64,
	pub max_power_yesterday: i64,
	pub day_sequence_no: i64,
}

#[derive(Debug)]
pub enum VictronError {
    NoError = 0,
    BatteryVoltageTooHigh = 2,
    ChargerTemperatureTooHigh = 17,
    ChargerOverCurrent = 18,
    ChargerCurrentReversed = 19,
    BulkTimeLimitExceeded = 20,
    CurrentSensorError = 21,
    TerminalsOverheated = 26,
    ConverterIssue = 28,
    InputVoltageTooHigh = 33,
    InputCurrentTooHigh = 34,
    InputShutdownVbatTooHigh = 38,
    InputShutdownCurrentDuringOffMode = 39,
    CommunicationError = 65,
    SynchronisedChargingConfigurationIssue = 66,
    BmsConnectionLost = 67,
    NetworkMisconfigured = 68,
    FactoryCalibrationDataLost = 116,
    InvalidFirmware = 117,
    UserSettingsInvalid = 119,
}

impl VictronFrame{
    pub fn from_json_value(jvalue: Value) -> Option<VictronFrame>{
        Some(Self {
            pid: jvalue["PID"].as_str().unwrap().to_string(),
            fw: jvalue["FW"].as_str().unwrap().parse::<i64>().unwrap(),
            serial_no: jvalue["SER#"].as_str().unwrap().to_string(),
            v_bat: jvalue["V"].as_str().unwrap().parse::<i64>().unwrap(),
            i_bat: jvalue["I"].as_str().unwrap().parse::<i64>().unwrap(),
            v_pv: jvalue["VPV"].as_str().unwrap().parse::<i64>().unwrap(),
            p_pv: jvalue["PPV"].as_str().unwrap().parse::<i64>().unwrap(),
            state: jvalue["CS"].as_str().unwrap().parse::<i64>().unwrap(), 
            mppt_mode: jvalue["MPPT"].as_str().unwrap().parse::<i64>().unwrap(), 
            error: jvalue["ERR"].as_str().unwrap().parse::<i64>().unwrap(),
            load_active: jvalue["LOAD"].as_str() == Some("ON"),
            i_load: jvalue["IL"].as_str().unwrap().parse::<i64>().unwrap(),
            yield_total: jvalue["H19"].as_str().unwrap().parse::<i64>().unwrap(),
            yield_today: jvalue["H20"].as_str().unwrap().parse::<i64>().unwrap(),
            max_power_today: jvalue["H21"].as_str().unwrap().parse::<i64>().unwrap(),
            yield_yesterday: jvalue["H22"].as_str().unwrap().parse::<i64>().unwrap(),
            max_power_yesterday: jvalue["H23"].as_str().unwrap().parse::<i64>().unwrap(),
            day_sequence_no: jvalue["HSDS"].as_str().unwrap().parse::<i64>().unwrap(),
        })
    }
}
