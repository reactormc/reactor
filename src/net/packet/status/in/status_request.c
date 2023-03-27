#include "status_request.h"
#include "../out/status_response.h"
#include "../../../../util/logger.h"

#include <json-c/json.h>

void handle_status_request(ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer) {
    debug("handle_status_request: creating json status response\n");

    /* json response building {{{2 */
    /* version */
    struct json_object *version = json_object_new_object();
    json_object_object_add(version, "name", json_object_new_string("1.19.4"));
    json_object_object_add(version, "protocol", json_object_new_int(762));
    /* end version */

    /* players */
    struct json_object *players = json_object_new_object();
    json_object_object_add(players, "max", json_object_new_int(420));
    json_object_object_add(players, "online", json_object_new_int(69));
    /* players->sample */
    struct json_object *sample = json_object_new_array_ext(2);
    struct json_object *firstPlayer = json_object_new_object();
    json_object_object_add(firstPlayer, "name", json_object_new_string("2008Choco"));
    json_object_object_add(firstPlayer, "id", json_object_new_string("73c62196-2af7-463d-8be1-a7a2270f4696"));
    struct json_object *secondPlayer = json_object_new_object();
    json_object_object_add(secondPlayer, "name", json_object_new_string("md_5"));
    json_object_object_add(secondPlayer, "id", json_object_new_string("af74a02d-19cb-445b-b07f-6866a861f783"));
    json_object_array_add(sample, firstPlayer);
    json_object_array_add(sample, secondPlayer);
    json_object_object_add(players, "sample", sample);
    /* end players->sample */
    /* end players */

    /* description */
    struct json_object *description = json_object_new_object();
    json_object_object_add(description, "text",
                           json_object_new_string("Welcome to the most 420/69 server in existence.\n"));
    /* end description */

    /* favicon */
    /* do not ask */
    struct json_object *favicon = json_object_new_string(
            "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAACXBIWXMAAAsTAAALEwEAmpwYAAAEwElEQVR4nO2ZYWwTZRjHn3Y4IQu6rcPeLcjsSsWx6cIGYoagRjAQxKkRERngN5e7dzV8wC8aQ2JEP/lBg8bERBP95AcS27lkRKMiG93o2OrGXVfB0bXbOmFsMtzo0D7mvdHm1vZa1pp4d7tf8mb9sCb3+z/PPe97VwADAwMDAwPdsubnyzZLj1gOSwpEs70reKy2d3R8TWcwCicFCywF1g9goaM79PnG/sjNrYNXsOJsEMElfgdLgUrP8Jv1/ZEZKk6XzRNEcIsIrcJu0DMPeIKNtb6xK3HxROWpvFscAEQT6BFr+1iR41zo+8f9fyTEF1ReWsIh0CO2rvDeul/HEu2uIO+Db7AA9ERF71BxlTf8U3LVU+VFBJfwDOgJW2dwT3y6Jy/ZPR+vfhvoCUd36NMGMbXqaSvvFqPw7YVq0EvLr/eO9KUTV5Cnrf8O6AHbL5d2yPf1O5KXBt9AIWgde9fwhw3CeExJPvWep5UXb4FLqANNg2he5w23KYkrV16q/rugZdZ6AvfUnA//nqO8D9oCd4NWuf9M0L7BNzaZm7w4Da2DVaBVKjsuP72pPxLNUR6h1d8EWqWyK9j8mDD+T87yLuET0Cpru4MfbRGVxbPKS1ve8ArQIuu8oVOZxKWtrjNj5aegLWAHtUAYfJu34rNZ/xHRXOUNd2eTz9L2MWgVXgS1wLN4jLCIPINRjsXGTK+rqs+P+POSnz/wvAVqkye3l1IIdI9/pG90JK+2n5f/EtTU9kQmLwvhpvx2oPK1vSOR/6Dyp1V12OEY3EkYnFUIQeoE1usvq+0bvZq/vHARXP4yUBtchhBet87NNThDM3nLu8Wr4L7gALXCpQmhmfkb6+7yYaWpA7e1hPKRj4JbfBLUDpcUQuO9EbTBGWnREDa/8FsOA0+IgUt8FbQClxTCrpWhRAh0bdoTWEzl6dA7ClqDk4VwsGwa7abOlBDuTF54D7QKx+DOg5brcw+aPQvk46tmQxZ5t/CFpn/NcbJY/1LJxKzd1JE2ACmEer9S25/U9I8ZnBU3Exan6C3wcukEZgqhemNKAG5Nv9B0slhPGLwm3w73lV5LmQOJ3QE60NYUiMufgh+HloOe5IlCCHaQfTZ1omP/xSFo7ysCPcqTpBAcprPYVHY9sUXWFfqw2Xorypfjc6BXeXJ77S2ZmD1g+XNOfliiJ8b4s4PmQnAuQp4ORjogsz1AaSYEZw7y8e9qPgRnHvKaDeGoBVe2MPjEvpLJ44ctN6bzkY9DX5rQlydpvhfjGWwGNXBkNZYSFj+jF0qnN53i0iGm4Bw2Fo/nLJ+hE2KERQJqgFuFDGFwkF6YXF6+niq6lLN8mhDUI0/hWfxBuifZGNYs61E80j6/sBOmiBUfhUVCrLhbNW1PISxulVf1tVUz+JC5K/2ZvsCbU+VVDWHwg+TWzhTCobIbf+lGnsKz+FW64ZYuBDobXimZfB/0BM/iCaUJT0OoKuhOyNMByTG4DfREC4sHMu3xNISHl/VI8nSLpOcE0BOHK3A5YXA4Uwh0d5j/iydAj5By3E5YTDy9KRxb/W9UYDHoFVKO23kGQwry7c770Ap658hqXEFY3M8z+DHP4NeEweOExS3/93UZGBgYGED+/AuD903B4vUcMQAAAABJRU5ErkJggg==");
    /* end favicon */

    /* enforcesSecureChat */
    struct json_object *enforcesSecureChat = json_object_new_boolean(0);
    /* end enforcesSecureChat */

    /* final message */
    struct json_object *response = json_object_new_object();
    json_object_object_add(response, "version", version);
    json_object_object_add(response, "players", players);
    json_object_object_add(response, "description", description);
    json_object_object_add(response, "favicon", favicon);
    json_object_object_add(response, "enforcesSecureChat", enforcesSecureChat);
    /* end final message */
    /* }}}2 */

    const char *data = json_object_to_json_string(response);
    uint8_t *out = encode_ntls_to_lpus(data, 32767); // max length of json response

    /* dispose object */
    json_object_put(response);

    send_status_response(conn, create_status_response(out));
}
