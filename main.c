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
    int weathersSize;
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

void printJsonResponse(struct json_object *jsonObject)
{
    puts("******** JSON RESPONSE ********");
    puts("");
    const char *jsonString = json_object_to_json_string_ext(jsonObject, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
    printf("%s\n", jsonString);
}

void printResponseStruct(Response response)
{
    puts("******** STRUCT RESPONSE ********");
    puts("");
    printf("coord:\n\tlon : %.2f\n\tlat : %.2f\n", response.coord.lon, response.coord.lat);
    printf("weathers:\n");
    for (size_t i = 0; i < response.weathersSize; i++)
    {
        printf("\t[id : %d, main : %s, description : %s, icon : %s]\n", response.weathers[i].id, response.weathers[i].main, response.weathers[i].description, response.weathers[i].icon);
    }
    printf("base : %s\n", response.base);
    printf("main:\n\ttemp : %.2f\n\tfeels_like : %.2f\n\ttemp_min : %.2f\n\ttemp_max : %.2f\n\tpressure : %.2f\n\thumidity : %.2f\n\tsea_level : %.2f\n\tgrnd_level : %.2f\n", response.main.temp, response.main.feelsLike, response.main.tempMin, response.main.tempMax, response.main.pressure, response.main.humidity, response.main.seaLevel, response.main.grndLevel);
    printf("visibility: %i\n", response.visibility);
    printf("wind:\n\tspeed : %.2f\n\tdeg: %.2f\n\tgust: %.2f\n", response.wind.speed, response.wind.deg, response.wind.gust);
    printf("clouds:\n\tall : %.2f\n", response.clouds.all);
    printf("dt: %i\n", response.dt);
    printf("sys:\n\ttype : %i\n\tid : %i\n\tcountry : %s\n\tsunrise : %i\n\tsunset : %i\n", response.sys.type, response.sys.id, response.sys.country, response.sys.sunrise, response.sys.sunset);
    printf("timezone : %i\n", response.timezone);
    printf("id : %i\n", response.id);
    printf("name : %s\n", response.name);
    printf("cod : %i\n", response.cod);
}

void parseJsonResponse(struct json_object *parsedJson, Response *response, Weather **weathers)
{
    struct json_object *coord, *weather, *main, *wind, *clouds, *sys, *base;
    struct json_object *visibility, *id, *dt, *name, *cod, *timezone;

    // Coord object
    json_object_object_get_ex(parsedJson, "coord", &coord);
    response->coord.lon = json_object_get_double(json_object_object_get(coord, "lon"));
    response->coord.lat = json_object_get_double(json_object_object_get(coord, "lat"));

    json_object_object_get_ex(parsedJson, "weather", &weather);
    int arrayLength = json_object_array_length(weather);
    array_list *weatherArray = json_object_get_array(weather);

    *weathers = calloc(arrayLength, sizeof(Weather));
    response->weathers = *weathers;
    response->weathersSize = arrayLength;

    for (int i = 0; i < arrayLength; i++)
    {
        // Get each item in the array
        struct json_object *weatherItem = (struct json_object *)array_list_get_idx(weatherArray, i);

        // Extract information from each weather item
        response->weathers[i].description = (char *)json_object_get_string(json_object_object_get(weatherItem, "description"));
        response->weathers[i].icon = (char *)json_object_get_string(json_object_object_get(weatherItem, "icon"));
        response->weathers[i].id = json_object_get_int(json_object_object_get(weatherItem, "id"));
        response->weathers[i].main = (char *)json_object_get_string(json_object_object_get(weatherItem, "main"));
    }

    json_object_object_get_ex(parsedJson, "base", &base);
    response->base = (char *)json_object_get_string(base);

    json_object_object_get_ex(parsedJson, "main", &main);
    response->main.temp = json_object_get_double(json_object_object_get(main, "temp"));
    response->main.feelsLike = json_object_get_double(json_object_object_get(main, "feels_like"));
    response->main.tempMin = json_object_get_double(json_object_object_get(main, "temp_min"));
    response->main.tempMax = json_object_get_double(json_object_object_get(main, "temp_max"));
    response->main.pressure = json_object_get_double(json_object_object_get(main, "pressure"));
    response->main.humidity = json_object_get_double(json_object_object_get(main, "humidity"));
    response->main.seaLevel = json_object_get_double(json_object_object_get(main, "sea_level"));
    response->main.grndLevel = json_object_get_double(json_object_object_get(main, "grnd_level"));

    json_object_object_get_ex(parsedJson, "visibility", &visibility);
    response->visibility = json_object_get_int(visibility);

    json_object_object_get_ex(parsedJson, "wind", &wind);
    response->wind.speed = json_object_get_double(json_object_object_get(wind, "speed"));
    response->wind.deg = json_object_get_double(json_object_object_get(wind, "deg"));
    response->wind.gust = json_object_get_double(json_object_object_get(wind, "gust"));

    json_object_object_get_ex(parsedJson, "clouds", &clouds);
    response->clouds.all = json_object_get_double(json_object_object_get(clouds, "all"));

    json_object_object_get_ex(parsedJson, "dt", &dt);
    response->dt = json_object_get_int(dt);

    json_object_object_get_ex(parsedJson, "sys", &sys);
    response->sys.type = json_object_get_int(json_object_object_get(sys, "type"));
    response->sys.id = json_object_get_int(json_object_object_get(sys, "id"));
    response->sys.country = (char *)json_object_get_string(json_object_object_get(sys, "country"));
    response->sys.sunrise = json_object_get_int(json_object_object_get(sys, "sunrise"));
    response->sys.sunset = json_object_get_int(json_object_object_get(sys, "sunset"));

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
            Response responseStruct;
            Weather *weathers = NULL;
            struct json_object *jsonObject = json_tokener_parse(info.data);
            printJsonResponse(jsonObject);
            parseJsonResponse(jsonObject, &responseStruct, &weathers);
            printResponseStruct(responseStruct);
            free(weathers);
            json_object_put(jsonObject);
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        free(info.data);
    }

    curl_global_cleanup();

    return 0;
}