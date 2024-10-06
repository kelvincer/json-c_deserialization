#include <stdlib.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <string.h>

typedef struct Coord
{
    float lon;
    float lat;

} Coord;

typedef struct Weather
{
    int id;
    char *main;
    char *description;
    char *icon;

} Weather;

typedef struct Main
{
    float temp;
    float feelsLike;
    float tempMin;
    float tempMax;
    float pressure;
    float humidity;
    float seaLevel;
    float grndLevel;

} Main;

typedef struct Wind
{
    float speed;
    float deg;
    float gust;
} Wind;

typedef struct Clouds
{
    float all;
} Clouds;

typedef struct Sys
{
    int type;
    int id;
    char *country;
    int sunrise;
    int sunset;
} Sys;

typedef struct Response
{
    Coord coord;
    Weather *weathers;
    char *base;
    Main main;
    int visibility;
    Wind wind;
    Clouds clouds;
    int dt;
    Sys sys;
    int timezone;
    int id;
    char *name;
    int cod;
} Response;

typedef struct Info
{
    char *data;
    size_t size;
} Info;

void parseJsonResponse(struct json_object *parsedJson, Response *response, Weather **weathers)
{
    // struct json_object *parsedJson;
    struct json_object *coord, *weather, *main, *wind, *clouds, *sys, *base;
    struct json_object *temp, *feelsLike, *tempMin, *tempMax, *pressure, *humidity;
    struct json_object *weatherDescription, *id, *icon;
    struct json_object *visibility, *country, *speed, *deg, *gust;
    struct json_object *all, *dt, *type, *sunrise, *sunset, *name, *cod, *timezone;

    // Coord object
    json_object_object_get_ex(parsedJson, "coord", &coord);
    response->coord.lon = json_object_get_double(json_object_object_get(coord, "lon"));
    response->coord.lat = json_object_get_double(json_object_object_get(coord, "lat"));

    json_object_object_get_ex(parsedJson, "weather", &weather);
    int arrayLength = json_object_array_length(weather);
    array_list *weatherArray = json_object_get_array(weather);

    *weathers = calloc(arrayLength, sizeof(Weather));
    response->weathers = *weathers;

    for (int i = 0; i < arrayLength; i++)
    {
        // Get each item in the array
        struct json_object *weatherItem = (struct json_object *)array_list_get_idx(weatherArray, i);

        // Extract information from each weather item
        json_object_object_get_ex(weatherItem, "description", &weatherDescription);
        json_object_object_get_ex(weatherItem, "id", &id);
        json_object_object_get_ex(weatherItem, "icon", &icon);
        json_object_object_get_ex(weatherItem, "main", &main);
        response->weathers[i].description = (char *)json_object_get_string(weatherDescription);
        response->weathers[i].icon = (char *)json_object_get_string(icon);
        response->weathers[i].id = json_object_get_int(id);
        response->weathers[i].main = (char *)json_object_get_string(main);
    }

    json_object_object_get_ex(parsedJson, "base", &base);
    response->base = (char *)json_object_get_string(base);

    json_object_object_get_ex(parsedJson, "main", &main);
    json_object_object_get_ex(main, "temp", &temp);
    json_object_object_get_ex(main, "feels_like", &feelsLike);
    json_object_object_get_ex(main, "temp_min", &tempMin);
    json_object_object_get_ex(main, "temp_max", &tempMax);
    json_object_object_get_ex(main, "pressure", &pressure);
    json_object_object_get_ex(main, "humidity", &humidity);

    response->main.temp = json_object_get_double(temp);
    response->main.feelsLike = json_object_get_double(feelsLike);
    response->main.tempMin = json_object_get_double(tempMin);
    response->main.tempMax = json_object_get_double(tempMax);
    response->main.pressure = json_object_get_double(pressure);
    response->main.humidity = json_object_get_double(humidity);

    json_object_object_get_ex(parsedJson, "visibility", &visibility);
    response->visibility = json_object_get_int(visibility);

    json_object_object_get_ex(parsedJson, "wind", &wind);
    json_object_object_get_ex(wind, "speed", &speed);
    json_object_object_get_ex(wind, "deg", &deg);
    json_object_object_get_ex(wind, "gust", &gust);
    response->wind.deg = json_object_get_double(speed);
    response->wind.speed = json_object_get_double(speed);
    response->wind.gust = json_object_get_double(gust);

    json_object_object_get_ex(parsedJson, "clouds", &clouds);
    json_object_object_get_ex(clouds, "all", &all);
    response->clouds.all = json_object_get_double(all);

    json_object_object_get_ex(parsedJson, "dt", &dt);
    response->dt = json_object_get_int(dt);

    json_object_object_get_ex(parsedJson, "sys", &sys);
    json_object_object_get_ex(sys, "type", &type);
    json_object_object_get_ex(sys, "id", &id);
    json_object_object_get_ex(sys, "country", &country);
    json_object_object_get_ex(sys, "sunrise", &sunrise);
    json_object_object_get_ex(sys, "sunset", &sunset);
    response->sys.type = json_object_get_int(type);
    response->sys.id = json_object_get_int(id);
    response->sys.country = (char *)json_object_get_string(country);
    response->sys.sunrise = json_object_get_int(sunrise);
    response->sys.sunset = json_object_get_int(sunset);

    json_object_object_get_ex(parsedJson, "timezone", &timezone);
    response->timezone = json_object_get_int(timezone);

    json_object_object_get_ex(parsedJson, "id", &id);
    response->id = json_object_get_int(id);

    json_object_object_get_ex(parsedJson, "name", &name);
    response->name = (char *)json_object_get_string(name);

    json_object_object_get_ex(parsedJson, "cod", &cod);
    response->cod = json_object_get_int(cod);
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, Info *s)
{
    size_t new_size = s->size + size * nmemb;
    s->data = realloc(s->data, new_size + 1);
    if (s->data == NULL)
    {
        fprintf(stderr, "realloc() failed\n");
        return 0;
    }
    memcpy(s->data, ptr, size * nmemb);
    s->data[new_size] = '\0';
    s->size = new_size;

    return size * nmemb;
}

int main()
{
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();

    char fullURL[300];
    char *URL = "https://api.openweathermap.org/data/2.5/weather?lat=44.34&lon=10.99&appid=%s";

    sprintf(fullURL, URL, "23337291985ee7ef7b4e584e35900d3b");

    if (curl)
    {
        Info info = {.data = NULL, .size = 0};

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(curl, CURLOPT_URL, fullURL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &info);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "accept: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode ret = curl_easy_perform(curl);

        if (ret != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(ret));
        }
        else
        {
            Response response;
            Weather *weathers = NULL;
            struct json_object *parsedJson = json_tokener_parse(info.data);
            parseJsonResponse(parsedJson, &response, &weathers);
            printf("code = %i, clouds = %f, name = %s, descrip = %s\n", response.cod, response.clouds.all, response.name, response.weathers[0].description);
            free(weathers);
            json_object_put(parsedJson);
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        free(info.data);
    }

    curl_global_cleanup();

    return 0;
}