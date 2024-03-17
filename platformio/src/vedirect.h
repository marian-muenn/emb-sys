#define VEDIRECT_H
typedef struct vedirect_frame_t {
	uint32_t vbat;
	uint32_t vpv;
  uint32_t ppv;
	uint32_t ibat;
	uint32_t iload;
	bool     load_switch;
	bool     relay_state;
	uint32_t off_reason;
	uint32_t yield_total;
	uint32_t yield_today;
	uint32_t max_power_today;
	uint32_t yield_yesterday;
	uint32_t max_power_yesterday;
	uint32_t error_code;
	uint32_t current_state;
	String*  firmware_version;
	String*  product_id;
	String*  serial_no;
	uint32_t day_sequence_no;
	uint32_t mppt_tracker_mode;
} vedirect_frame;

String serialize_frame(vedirect_frame* f){
	String s = "{\n  \"v_bat\": ";
	s += f->vbat;
	
	s += ",\n  \"v_pv\": ";
	s += f->vpv;;

	s += ",\n  \"i_bat\": ";
	s += f->ibat;
	
	s += ",\n  \"i_load\": ";
	s += f->iload;

	s += ",\n  \"load_switch\": ";
	s += f->load_switch;

	s += ",\n  \"relay_state\": ";
	s += f->relay_state;

	s += ",\n  \"off_reason\": ";
	s += f->off_reason;

	s += ",\n  \"yield_total\": ";
	s += f->yield_total;
	
	s += ",\n  \"yield_today\": ";
	s += f->yield_today;
	
	s += ",\n  \"max_power_today\": ";
	s += f->max_power_today;
	
	s += ",\n  \"yield_yesterday\": ";
	s += f->yield_yesterday;
	
	s += ",\n  \"max_power_yesterday\": ";
	s += f->max_power_yesterday;

	s += ",\n  \"error_code\": ";
	s += f->error_code;
	
	s += ",\n  \"current_state\": ";
	s += f->current_state;

	s += ",\n  \"firmware_version\": ";
	s += *(f->firmware_version);
	
	s += ",\n  \"product_id\": ";
	s += *(f->product_id);

	s += ",\n  \"serial_no\": ";
	s += *(f->serial_no);
	
	s += ",\n  \"day_sequence_no\": ";
	s += f->day_sequence_no;

	s += ",\n  \"mppt_tracker_mode\": ";
	s += f->mppt_tracker_mode;;
	
	s += "\n}";
	return s;
}
typedef struct vedirect_parser_t {
  bool await_frame_start;
  vedirect_frame frame;
  char linebuffer[128];
  size_t linebuffer_len;
} vedirect_parser;

vedirect_parser* new_parser(){
  vedirect_parser* parser = new vedirect_parser;
  parser->await_frame_start = true;
  parser->frame.vbat = 0;
  parser->frame.vpv = 0;
  parser->frame.ppv = 0;
  parser->frame.ibat= 0;
  parser->frame.iload = 0;
  parser->frame.load_switch = 0;
  parser->frame.relay_state = 0;
  parser->frame.off_reason = 0;
  parser->frame.yield_total = 0;
  parser->frame.max_power_today = 0;
  parser->frame.max_power_yesterday = 0;
  parser->frame.error_code = 0;
  parser->frame.current_state = 0;
  parser->frame.firmware_version = new String("");
  parser->frame.product_id = new String("");
  parser->frame.serial_no = new String("");
  parser->frame.day_sequence_no = 0;
  parser->frame.mppt_tracker_mode = 0;
  return parser;
}

void parse_line(vedirect_parser_t* parser){
  uint tabs = 0;
  size_t i = 0;
  String key = "";
  String value = "";
  while (i < parser->linebuffer_len){
    if (parser->linebuffer[i] == '\t'){
      tabs += 1;
    }
  }
  // Correct Line has 1 Tab
  if (tabs == 1){
    // parse key
    i = 0;
    while(i < parser->linebuffer_len && parser->linebuffer[i] != '\t'){
      key += parser->linebuffer[i];
    };
    // parse value
    while(i < parser->linebuffer_len-2){
      value += parser->linebuffer[i];
    };
    // Checksum is the last line of a frame
    if (parser->await_frame_start && value == "Checksum"){
      parser->await_frame_start = false;
      return;
    }
    // Last Line received
    if (!parser->await_frame_start && key == "Checksum"){
      String msg = serialize_frame(&(parser->frame));
      Serial.write(msg.c_str());
      extern PubSubClient client;
      client.publish(MQTT_DATA_TOPIC, msg.c_str(), msg.length());
    }
    else {
      // v_bat
      if (key == "V")
        parser->frame.vbat = value.toInt();
      else if (key == "VPV")
        parser->frame.vpv = value.toInt();
      else if (key == "PPV")
        parser->frame.ppv = value.toInt();
      /*
      "I"
      "IL"
      "LOAD"
      "RELAY"
      "OR"
      "H19"
      "H20"
      "H21"
      "H22"
      "H23"
      "ERR"
      "CS"
      "FW"
      "PID"
      "ser#"
      "hsds"
      "mppt"
      */
    }
  }
}

void parse_char(vedirect_parser_t* parser, char c){
  // parse content of linebuffer if last chars are crlf
  if (c == '\n' && parser->linebuffer[parser->linebuffer_len - 1] == '\r' ){
    parse_line(parser);
  } else if (parser->linebuffer_len < 128){
    parser->linebuffer[parser->linebuffer_len] = c;
    parser->linebuffer_len+=1;
  // reset buffer if length reaches max
  } else parser->linebuffer_len = 0;
}

