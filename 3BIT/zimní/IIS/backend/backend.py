#!/usr/bin/env python3

import pymysql.cursors
import configparser
import time
from flask import Flask, request, jsonify, abort, make_response
from passlib.hash import sha256_crypt
from flask_cors import CORS
from functools import wraps
from secrets import token_hex
    
# inicializace config parseru     
config = configparser.ConfigParser()
config.read('config.ini')
# FLASK
app = Flask(__name__)
# nastaveni CROS, pro API
CORS(app)


table_uzivatel = {"jmeno", "heslo", "opravneni", "email"}
table_druh = {"dodavatel", "kvalita", "louhavani", "charakter", "jmeno_caje", "kategorie"}
table_zeme = {"nazev_zeme", "kontinent"}
table_oblasti = {"nazev_zeme", "nazev_oblasti"}
table_odberatel = {"odberatel_id", "telefon", "email", "osloveni", "ulice", "mesto", "psc"}
table_objednavka = {"objednavka_id", "odberatel_id", "stav"}
table_varka = {"varka_id", "cena", "zbyvajici_mnozstvi", "sleva"}
table_fyzicka_osoba = {"odberatel_id", "jmeno", "prijmeni", "osobni_telefon"}
table_pravnicka_osoba = {"odberatel_id", "jmeno_firmy", "ic", "dic"}
identification = ("token", "level")

active = dict()
login_expiration = 60 * 60 * 24

connection = pymysql.connect(host=config['DB']['HOST'],
                             user=config['DB']['USER'],
                             password=config['DB']['PASSWORD'],
                             db=config['DB']['DB_NAME'],
                             charset=config['DB']['CHARSET'],
                             cursorclass=pymysql.cursors.DictCursor)


def login_level_required(level=0):
    def decorator_wraps(f):
        @wraps(f)
        def decorated_function(*args, **kwargs):
            if not request.headers:
                print("1")
                abort(403)
            if identification[0] not in request.headers:
                print("2")
                abort(403)
            if request.headers[identification[0]] not in active:
                print("3")
                abort(403)
            if active[request.headers[identification[0]]][1] < level:
                print("4")
                abort(403)
            if active[request.headers[identification[0]]][2] < time.time():
                print("5")
                active.pop(request.headers[identification[0]])
                abort(403)

            active[request.headers[identification[0]]][2] = time.time() + login_expiration
            return f(*args, **kwargs)
        return decorated_function
    return decorator_wraps


############################
# User
############################


@app.route('/user/', methods=['GET'])
@login_level_required(5)
def user_get():

    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM uzivatel;")

    return jsonify(cursor.fetchall())


@app.route('/user/', methods=['POST'])
@login_level_required(5)
def user_post():

    with connection.cursor() as cursor:
        if not request.json:
            abort(400)

        for i in table_uzivatel:
            if i not in request.json:
                abort(400)

        sql = "INSERT INTO uzivatel (jmeno, heslo, opravneni, email) VALUES(%s, %s, %s, %s);"
        values = (request.json["jmeno"], sha256_crypt.encrypt(request.json["heslo"]),
                  request.json["opravneni"], request.json["email"])

        try:
            cursor.execute(sql, values)
            connection.commit()
        except pymysql.err.IntegrityError:
            return "Duplicitní záznam\n", 400
        except pymysql.err.InternalError:
            return "Nevalidní hodnoty\n", 400
        except pymysql.err.DataError:
            return "Nadměrné hodnoty\n", 400

    return "", 201


@app.route('/user/', methods=['PUT'])
@login_level_required(5)
def user_put():
    msg = ""

    with connection.cursor() as cursor:
        if not request.json or not request.json.get("email"):
            abort(400)

        for i in request.json:
            if i not in table_uzivatel:
                abort(400)

        cursor.execute("SELECT * FROM uzivatel WHERE email = %s", (request.json["email"], ))
        if not cursor.fetchall():
            return "Uzivatel nenalezen\n", 400

        for i in request.json:
            if i == "email" or not request.json[i]:
                continue

            if i == "heslo":
                sql = "UPDATE uzivatel SET heslo = %s WHERE email = %s;"
                values = (sha256_crypt.encrypt(request.json[i]), request.json["email"])
            else:
                sql = "UPDATE uzivatel SET {} = %s WHERE email = %s;".format(i)
                values = (request.json[i], request.json["email"])
            try:
                cursor.execute(sql, values)
            except pymysql.err.InternalError:
                msg = "{}hodnota {} byla zahozena (nevalidni hodnota)\n".format(msg, i)
            except pymysql.err.DataError:
                msg = "{}hodnota {} byla zahozena (nadmerne data)\n".format(msg, i)
        connection.commit()

    return msg, 202


@app.route('/user/', methods=['DELETE'])
@login_level_required(5)
def user_delete():

    with connection.cursor() as cursor:
        if not request.json or not request.json.get("email"):
            abort(400)

        sql = "DELETE FROM uzivatel WHERE email = %s;"
        if cursor.execute(sql, (request.json["email"], )):
            connection.commit()
            return "", 202
        else:
            return "Uzivatel neexistuje\n", 400


############################
# Categories
############################


# @admin_level_required
@app.route('/categories/', methods=['GET'])
@app.route('/categories/<string:category_id>', methods=['GET'])
@login_level_required(1)
def categories_get(category_id=None):

    with connection.cursor() as cursor:
        if category_id:
            sql = "SELECT * FROM druh AS d NATURAL JOIN druh_ze_zeme NATURAL JOIN zeme NATURAL JOIN oblasti " \
                  "WHERE kategorie = %s AND EXISTS(SELECT * FROM varka_z_druhu AS vzd WHERE vzd.druh_id = d.druh_id);"
            if not cursor.execute(sql, (category_id, )):
                return "Kategorie neexistuje\n", 400
        else:
            cursor.execute("SELECT DISTINCT kategorie FROM druh WHERE EXISTS(SELECT * FROM varka_z_druhu AS vzd "
                           "WHERE vzd.druh_id = d.druh_id);")

    return jsonify(cursor.fetchall())


############################
# Tea
############################


@app.route('/tea/', methods=['GET'])
@app.route('/tea/<int:tea_id>', methods=['GET'])
@login_level_required(1)
def tea_get(tea_id=None):

    with connection.cursor() as cursor:
        if tea_id:
            sql = "SELECT * FROM druh NATURAL JOIN druh_ze_zeme NATURAL JOIN zeme NATURAL JOIN oblasti " \
                  "WHERE druh_id = %s;"
            cursor.execute(sql, (tea_id, ))

            druh = cursor.fetchone()

            sql = "SELECT varka_id, cena, zbyvajici_mnozstvi, sleva FROM varka NATURAL JOIN varka_z_druhu  " \
                  "WHERE druh_id = %s;"
            cursor.execute(sql, (tea_id, ))

            druh["vakra"] = cursor.fetchall()
        else:
            cursor.execute("SELECT * FROM druh as d NATURAL JOIN druh_ze_zeme NATURAL JOIN oblasti WHERE EXISTS(SELECT * FROM varka_z_druhu AS vzd WHERE vzd.druh_id = d.druh_id);")
            druh = cursor.fetchall()

    return jsonify(druh)


@app.route('/tea/', methods=['POST'])
@login_level_required(4)
def tea_post():
    with connection.cursor() as cursor:
        if not request.json:
            abort(400)

        for i in (table_druh | table_varka | table_zeme | table_oblasti) - {"varka_id"}:
            if i not in request.json:
                abort(400)

        druh_id = request.json["druh_id"]
        cursor.execute("SELECT * FROM druh WHERE druh_id = %s", (request.json["druh_id"], ))
        if not cursor.fetchall():


            
            # druh
            sql = "INSERT INTO druh (dodavatel, kvalita, louhavani, charakter, jmeno_caje, kategorie, vyska, obrazek) " \
                  "VALUES(%s, %s, %s, %s, %s, %s, %s, %s);"
            values = (request.json["dodavatel"], int(request.json["kvalita"]),
                      int(request.json["louhavani"]), request.json["charakter"], request.json["jmeno_caje"], request.json["kategorie"], 1000, "dfassa")

            # pokud druh existuje, zahodime
            try:
                cursor.execute(sql, values)
            except pymysql.err.InternalError:
                return "a Nevalidni hodnoty\n", 400
            except pymysql.err.DataError:
                return "Nadmerne hodnoty\n", 400

            sql = "SELECT MAX(druh_id) FROM druh;"
            cursor.execute(sql)
            druh_id = cursor.fetchone().get("MAX(druh_id)")

        # zeme
        sql = "INSERT INTO zeme (nazev_zeme, kontinent) VALUES(%s, %s);"
        values = (request.json["nazev_zeme"], request.json["kontinent"])

        # pokud zeme existuje, zahodime
        try:
            cursor.execute(sql, values)
        except pymysql.err.IntegrityError:
            pass
        except pymysql.err.InternalError:
            return "b Nevalidni hodnoty\n", 400
        except pymysql.err.DataError:
            return "Nadmerne hodnoty\n", 400

        # oblast
        sql = "INSERT INTO oblasti (nazev_zeme, nazev_oblasti) VALUES(%s, %s);"
        values = (request.json["nazev_zeme"], request.json["nazev_oblasti"])

        # pokud oblast existuje, zahodime
        try:
            cursor.execute(sql, values)
        except pymysql.err.IntegrityError:
            pass
        except pymysql.err.InternalError:
            return "c Nevalidni hodnoty\n", 400
        except pymysql.err.DataError:
            return "Nadmerne hodnoty\n", 400

        # druh ze zeme
        sql = "INSERT INTO druh_ze_zeme (druh_id, nazev_zeme, nazev_oblasti) VALUES(%s, %s, %s);"
        values = (druh_id, request.json["nazev_zeme"], request.json["nazev_oblasti"])

        # pokud druh je jiz ze zeme, zahodime
        try:
            cursor.execute(sql, values)
        except pymysql.err.IntegrityError:
            pass
        except pymysql.err.InternalError:
            return "d Nevalidni hodnoty\n", 400
        except pymysql.err.DataError:
            return "Nadmerne hodnoty\n", 400

        # varka
        sql = "INSERT INTO varka (cena, zbyvajici_mnozstvi, sleva) VALUES(%s, %s, %s);"
        values = (request.json["cena"], request.json["zbyvajici_mnozstvi"], request.json["sleva"])
        try:
            cursor.execute(sql, values)
        except pymysql.err.InternalError:
            return "e Nevalidni hodnoty\n", 400
        except pymysql.err.DataError:
            return "Nadmerne hodnoty\n", 400

        # varka z druhu
        sql = "INSERT INTO varka_z_druhu (varka_id, druh_id) VALUES((SELECT MAX(varka_id) FROM varka), %s);"
        try:
            cursor.execute(sql, (druh_id, ))
        except pymysql.err.InternalError:
            return "f Nevalidni hodnoty\n", 400
        except pymysql.err.DataError:
            return "Nadmerne hodnoty\n", 400

        connection.commit()

    return jsonify({"druh_id": druh_id}), 201


@app.route('/tea/<int:tea_id>', methods=['PUT'])
@login_level_required(4)
def tea_put(tea_id):
    msg = ""

    with connection.cursor() as cursor:
        if not request.json:
            abort(400)

        for i in request.json:
            if i not in table_druh | table_varka:
                abort(400)

        # aktualizace druhu
        for i in request.json.keys() & table_druh:
            sql = "UPDATE druh SET {} = %s " \
                  "WHERE druh_id = (SELECT druh_id FROM varka_z_druhu WHERE varka_id = %s);".format(i)
            values = (request.json[i], tea_id)
            try:
                cursor.execute(sql, values)
            except pymysql.err.InternalError:
                msg = "{}hodnota {} byla zahozena (nevalidni hodnota)\n".format(msg, i)
            except pymysql.err.DataError:
                msg = "{}hodnota {} byla zahozena (nadmerne data)\n".format(msg, i)

        # aktualizace varky
        for i in request.json.keys() & table_varka:
            sql = "UPDATE varka SET {} = %s WHERE varka_id = %s;".format(i)
            values = (request.json[i], tea_id)
            try:
                cursor.execute(sql, values)
            except pymysql.err.InternalError:
                msg = "{}hodnota {} byla zahozena (nevalidni hodnota)\n".format(msg, i)
            except pymysql.err.DataError:
                msg = "{}hodnota {} byla zahozena (nadmerne data)\n".format(msg, i)

        connection.commit()

    return msg, 202


@app.route('/tea/<int:tea_id>', methods=['DELETE'])
@login_level_required(4)
def tea_delete(tea_id):

    with connection.cursor() as cursor:
        sql = "DELETE FROM varka WHERE varka_id = %s;"
        if cursor.execute(sql, (tea_id, )):
            connection.commit()
            return "", 202
        else:
            return "Caj neexistuje\n", 400


############################
# Order
############################


@app.route('/order/', methods=['GET'])
@app.route('/order/<int:order_id>', methods=['GET'])
@login_level_required(1)
def order_get(order_id=None):

    with connection.cursor() as cursor:
        if order_id:
            sql = "SELECT * FROM objednavka NATURAL JOIN odberatel WHERE objednavka_id = %s;"
            if not cursor.execute(sql, (order_id, )):
                return "Objednavka neexistuje\n", 400

            objednavka = cursor.fetchone()

            sql = "SELECT * FROM varka NATURAL JOIN varka_z_druhu NATURAL JOIN druh NATURAL JOIN varka_v_objednavce " \
                  "WHERE objednavka_id = %s;"
            cursor.execute(sql, (order_id,))

            objednavka["varka"] = cursor.fetchall()

        else:
            cursor.execute("SELECT * FROM objednavka NATURAL JOIN odberatel;")
            objednavka = cursor.fetchall()

    return jsonify(objednavka)


@app.route('/order/', methods=['POST'])
@login_level_required(1)
def order_post():

    if not request.json or "email" not in request.json or "varka_id" not in request.json or \
            "mnozstvi" not in request.json:
        abort(400)

    with connection.cursor() as cursor:

        cursor.execute("SELECT * FROM odberatel WHERE email = %s", (request.json["email"], ))
        if not cursor.fetchall():
            return "Odberatel nenalezen\n", 400

        # zjisteni odberatel_id
        cursor.execute("SELECT odberatel_id FROM odberatel WHERE odberatel.email = %s;", (request.json["email"], ))
        odberatel_id = cursor.fetchone()["odberatel_id"]

        cursor.execute("SELECT * FROM odberatel WHERE odberatel_id = %s", (odberatel_id, ))
        if not cursor.fetchall():
            return "Odberatel nenalezen\n", 400

        cursor.execute("SELECT zbyvajici_mnozstvi FROM varka WHERE varka_id = %s", (request.json["varka_id"], ))

        mnozstvi = cursor.fetchone()
        if not mnozstvi:
            return "Varka nenalezena\n", 400
        elif mnozstvi["zbyvajici_mnozstvi"] < int(request.json["mnozstvi"]):
            return "Tolik toho nemame\n", 400

        # ma odberatel objednavku, ktera jeste nebyla vyrizena
        sql = "SELECT MAX(objednavka_id) FROM objednavka WHERE odberatel_id = %s AND stav = 0;"
        cursor.execute(sql, (odberatel_id, ))

        objednavka_id = cursor.fetchone().get("MAX(objednavka_id)")

        # nova objednavka
        if not objednavka_id:
            sql = "INSERT INTO objednavka (odberatel_id, stav) VALUES(%s, %s);"
            cursor.execute(sql, (odberatel_id, '0'))

            cursor.execute("SELECT MAX(objednavka_id) FROM objednavka")
            objednavka_id = cursor.fetchone().get("MAX(objednavka_id)")

        # varka v objednavce
        try:
            sql = "INSERT INTO varka_v_objednavce (objednavka_id, varka_id, mnozstvi) VALUES(%s, %s, %s);"
            cursor.execute(sql, (objednavka_id, request.json["varka_id"], request.json["mnozstvi"]))
        except pymysql.err.IntegrityError:
            return "Varka jiz je v objednavce\n", 400
        except pymysql.err.InternalError:
            return "Nevalidni hodnota\n", 400
        except pymysql.err.DataError:
            return "Nadmerne data\n", 400

        connection.commit()

    return jsonify({"id": objednavka_id}), 201


@app.route('/order/<order_id>', methods=['PUT'])
@login_level_required(1)
def order_put(order_id):

    if not request.json:
        abort(400)

    for i in request.json:
        if i not in (table_objednavka | table_varka) - {"objednavka_id"}:
            abort(400)

    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM objednavka WHERE objednavka_id = %s", (order_id, ))
        if not cursor.fetchall():
            return "Objednavka nenalezena\n", 400

        if "stav" in request.json:
            cursor.execute("SELECT stav FROM objednavka WHERE objednavka_id = %s", (order_id, ))
            stav = cursor.fetchone().get("stav")
            if stav != 0:
                return "Objednavka jiz byla vyrizena\n", 400

            sql = "UPDATE objednavka SET stav = %s WHERE objednavka_id = %s;"
            values = (request.json["stav"], order_id)
            try:
                cursor.execute(sql, values)
            except pymysql.err.InternalError:
                return "Nevalidni hodnota\n", 400
            except pymysql.err.DataError:
                return "Nadmerne data\n", 400

            sql = "UPDATE varka AS var, (SELECT * FROM varka_v_objednavce WHERE objednavka_id = %s) AS vvo " \
                  "SET var.zbyvajici_mnozstvi = var.zbyvajici_mnozstvi - vvo.mnozstvi " \
                  "WHERE var.varka_id = vvo.varka_id;"
            cursor.execute(sql, (order_id, ))

        if "mnozstvi" in request.json and "varka_id" in request.json:

            cursor.execute("SELECT zbyvajici_mnozstvi FROM varka WHERE varka_id = %s", (request.json["varka_id"],))

            mnozstvi = cursor.fetchone()
            if not mnozstvi:
                return "Varka nenalezena\n", 400
            elif mnozstvi["zbyvajici_mnozstvi"] < int(request.json["mnozstvi"]):
                return "Tolik toho nemame\n", 400

            sql = "UPDATE varka_v_objednavce SET mnozstvi = %s WHERE objednavka_id = %s AND varka_id = %s;"
            values = (request.json["mnozstvi"], order_id, request.json["varka_id"])
            try:
                cursor.execute(sql, values)
            except pymysql.err.InternalError:
                return "Nevalidni hodnota\n", 400
            except pymysql.err.DataError:
                return "Nadmerne data\n", 400

        connection.commit()

    return "", 202


@app.route('/order/<int:order_id>', methods=['DELETE'])
@login_level_required(1)
def order_delete(order_id):

    with connection.cursor() as cursor:

        cursor.execute("SELECT * FROM objednavka WHERE objednavka_id = %s", (order_id,))
        if not cursor.fetchall():
            return "Objednavka nenalezena\n", 400

        if request.json and request.json.get("varka_id"):
            sql = "DELETE FROM varka_v_objednavce WHERE varka_id = %s and objednavka_id = %s;"
            try:
                cursor.execute(sql, (request.json.get("varka_id"), order_id))
            except pymysql.err.InternalError:
                return "Nevalidni hodnota\n", 400

        else:
            sql = "DELETE FROM objednavka WHERE objednavka_id = %s;"
            cursor.execute(sql, (order_id, ))

        connection.commit()

    return "", 202


############################
# Customer
############################


@app.route('/customer/', methods=['GET'])
@app.route('/customer/', methods=['GET'])
@login_level_required(4)
def customer_get():
    with connection.cursor() as cursor:

        cursor.execute("SELECT * FROM odberatel "
                       "LEFT JOIN fyzicka_osoba ON odberatel.odberatel_id = fyzicka_osoba.odberatel_id "
                       "LEFT JOIN pravnicka_osoba ON odberatel.odberatel_id = pravnicka_osoba.odberatel_id;")

    return jsonify(cursor.fetchall())


@app.route('/customer/', methods=['POST'])
@login_level_required(4)
def customer_post():
    with connection.cursor() as cursor:

        # zadna data
        if not request.json:
            abort(400)

        # data odberatele
        for i in table_odberatel - {"odberatel_id"}:
            if i not in request.json:
                abort(400)

        # vlozime odberatele
        try:
            cursor.execute("INSERT INTO odberatel (telefon, email, osloveni, ulice, mesto, psc) "
                           "VALUES(%s, %s, %s, %s, %s, %s);",
                           (int(request.json["telefon"]), request.json["email"], request.json["osloveni"],
                            request.json["ulice"], request.json["mesto"], int(request.json["psc"])))
        except pymysql.err.InternalError:
            return "Nevalidni hodnota\n", 400
        except pymysql.err.DataError:
            return "Nadmerne data\n", 400

        # zjisteni odberatel_id
        cursor.execute("SELECT MAX(odberatel_id) FROM odberatel;")
        odberatel_id = cursor.fetchone()["MAX(odberatel_id)"]

        # specializace odberatele
        if "ic" in request.json:

            for i in table_pravnicka_osoba - {"odberatel_id"}:
                if i not in request.json:
                    abort(400)

            sql = "INSERT INTO pravnicka_osoba (odberatel_id, jmeno_firmy, ic, dic) VALUES(%s, %s, %s, %s);"
            values = (odberatel_id, request.json["jmeno_firmy"], int(request.json["ic"]), request.json["dic"])

        else:

            for i in table_fyzicka_osoba - {"odberatel_id"}:
                if i not in request.json:
                    abort(400)

            sql = "INSERT INTO fyzicka_osoba (odberatel_id, jmeno, prijmeni, osobni_telefon) VALUES(%s, %s, %s, %s);"
            values = (odberatel_id, request.json["jmeno"], request.json["prijmeni"], request.json["osobni_telefon"])

        try:
            cursor.execute(sql, values)
        except pymysql.err.InternalError:
            cursor.execute("DELETE FROM odberatel WHERE odberatel_id = %s;", (odberatel_id, ))
            return "Nevalidni hodnota\n", 400
        except pymysql.err.DataError:
            cursor.execute("DELETE FROM odberatel WHERE odberatel_id = %s;", (odberatel_id, ))
            return "Nadmerne data\n", 400

        connection.commit()

        return "", 201


@app.route('/customer/', methods=['PUT'])
@login_level_required(4)
def customer_put():
    with connection.cursor() as cursor:
        if not request.json or not request.json.get("email"):
            abort(400)

        # neni nic navic
        for i in request.json:
            if i not in table_odberatel | table_fyzicka_osoba | table_pravnicka_osoba:
                abort(400)

        # nejsou data jak pro pravnickou tak fyzickou osobu
        if request.json.keys() - table_odberatel - table_fyzicka_osoba and \
                request.json.keys() - table_odberatel - table_pravnicka_osoba:
            abort(400)

        cursor.execute("SELECT * FROM odberatel WHERE email = %s", (request.json["email"], ))
        if not cursor.fetchall():
            return "Odberatel nenalezen\n", 400

        # zjisteni odberatel_id
        cursor.execute("SELECT odberatel_id FROM odberatel WHERE odberatel.email = %s;", (request.json["email"],))
        odberatel_id = cursor.fetchone()["odberatel_id"]

        for i in request.json.keys() & table_odberatel:
            if not request.json[i]:
                continue
            sql = "UPDATE odberatel SET {} = %s WHERE odberatel_id = %s;".format(i)
            try:
                cursor.execute(sql, (request.json[i], odberatel_id))
            except pymysql.err.InternalError:
                return "Nevalidni hodnota\n", 400
            except pymysql.err.DataError:
                return "Nadmerne data\n", 400

        for i in request.json.keys() & table_pravnicka_osoba:
            if not request.json[i]:
                continue
            sql = "UPDATE pravnicka_osoba SET {} = %s WHERE odberatel_id = %s;".format(i)
            try:
                cursor.execute(sql, (request.json[i], odberatel_id))
            except pymysql.err.InternalError:
                return "Nevalidni hodnota\n", 400
            except pymysql.err.DataError:
                return "Nadmerne data\n", 400

        for i in request.json.keys() & table_fyzicka_osoba:
            if not request.json[i]:
                continue
            sql = "UPDATE fyzicka_osoba SET {} = %s WHERE odberatel_id = %s;".format(i)
            try:
                cursor.execute(sql, (request.json[i], odberatel_id))
            except pymysql.err.InternalError:
                return "Nevalidni hodnota\n", 400
            except pymysql.err.DataError:
                return "Nadmerne data\n", 400

        connection.commit()

    return "", 202


@app.route('/customer/', methods=['DELETE'])
@login_level_required(4)
def customer_delete():

    with connection.cursor() as cursor:
        if not request.json or not request.json.get("email"):
            abort(400)

        sql = "DELETE FROM odberatel WHERE email = %s;"
        if cursor.execute(sql, (request.json["email"],)):
            connection.commit()
            return "", 202
        else:
            return "Uzivatel neexistuje\n", 400


############################
# Main
############################


@app.errorhandler(404)
def not_found(error):
    return make_response(jsonify({'error': 'Nenalezeno'}), 404)


@app.errorhandler(500)
def internal_error(error):
    return make_response(jsonify({'error': 'Neco je spatne'}), 500)


@app.route("/login", methods=["POST"])
def login():
    with connection.cursor() as cursor:

        # zadna data
        if not request.json or "email" not in request.json or "heslo" not in request.json:
            abort(400)

        cursor.execute("SELECT * FROM uzivatel WHERE email = %s;", (request.json["email"], ))
        user = cursor.fetchone()

        if user and sha256_crypt.verify(request.json.get("heslo"), user["heslo"]):

            token = token_hex(32)
            active[token] = [user["email"], user["opravneni"], time.time() + login_expiration]

            print(user)

            sql = "SELECT MAX(objednavka_id) FROM objednavka WHERE odberatel_id = " \
                  "(SELECT odberatel_id FROM odberatel WHERE email = %s) AND stav = 0;"
            cursor.execute(sql, (user["email"], ))

            objednavka_id = cursor.fetchone().get("MAX(objednavka_id)")

            if not objednavka_id:
                objednavka_id = -1

            return jsonify({
                identification[0]: token, 
                identification[1]: user["opravneni"], 
                "name": user["jmeno"],
                "email": user["email"],
                "order": objednavka_id
            }), 200

    return "", 403


@app.route("/logout", methods=['GET'])
@login_level_required(1)
def logout():
    active.pop(request.headers[identification[0]])

    return "", 200


if __name__ == "__main__":
    app.run(debug=True, threaded=False)
