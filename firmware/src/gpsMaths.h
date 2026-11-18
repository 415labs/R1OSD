// Maths

float deg2rad(float deg_angle);
float distance(float lat1, float long1, float lat2, float long2, float units_per_meter);
int course(float _sourceLat, float _sourceLong, float _destLat, float _destLong);
Position_t updateHemispheres( Position_t _position);
Aircraft_t updateBearingDataToHome(Aircraft_t _Aircraft);
Aircraft_t checkForMaxValues(Aircraft_t _Aircraft);

