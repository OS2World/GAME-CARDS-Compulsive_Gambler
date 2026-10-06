/* Caribbean Poker - language table. ASCII only (no accents).
   Order of the strings must match the enum in cplang.h. */

#include <os2.h>
#include "cplang.h"

int current_lang = LANG_EN;

const char *rank_sing[LANG_COUNT][13] = {
    { "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King", "Ace" },
    { "dos", "tres", "cuatro", "cinco", "seis", "siete", "ocho", "nueve", "diez", "jota", "reina", "rey", "as" },
    { "twee", "drie", "vier", "vijf", "zes", "zeven", "acht", "negen", "tien", "boer", "vrouw", "heer", "aas" },
    { "Zwei", "Drei", "Vier", "Fuenf", "Sechs", "Sieben", "Acht", "Neun", "Zehn", "Bube", "Dame", "Koenig", "Ass" },
    { "deux", "trois", "quatre", "cinq", "six", "sept", "huit", "neuf", "dix", "valet", "dame", "roi", "as" },
    { "due", "tre", "quattro", "cinque", "sei", "sette", "otto", "nove", "dieci", "fante", "donna", "re", "asso" }
};

const char *rank_plur[LANG_COUNT][13] = {
    { "Deuces", "Threes", "Fours", "Fives", "Sixes", "Sevens", "Eights", "Nines", "Tens", "Jacks", "Queens", "Kings", "Aces" },
    { "doses", "treses", "cuatros", "cincos", "seises", "sietes", "ochos", "nueves", "dieces", "jotas", "reinas", "reyes", "ases" },
    { "tweeen", "drieen", "vieren", "vijven", "zessen", "zevens", "achten", "negens", "tienen", "boeren", "vrouwen", "heren", "azen" },
    { "Zweien", "Dreien", "Vieren", "Fuenfen", "Sechsen", "Siebenen", "Achten", "Neunen", "Zehnen", "Buben", "Damen", "Koenige", "Asse" },
    { "deux", "trois", "quatre", "cinq", "six", "sept", "huit", "neuf", "dix", "valets", "dames", "rois", "as" },
    { "due", "tre", "quattro", "cinque", "sei", "sette", "otto", "nove", "dieci", "fanti", "donne", "re", "assi" }
};

const char *lang_strings[LANG_COUNT][S_COUNT] = {

/* ======================= LANG_EN ======================= */
{
    "~Game", "~New Game\tCtrl+N", "~Stud", "~Draw", "E~xit\tCtrl+X",
    "~Cash", "Add ~$100", "~Cash Out",
    "~Options", "~Sound", "~On", "O~ff",
    "~Language", "~Frame Controls\tCtrl+F", "S~ave settings on exit",
    "~Help", "~General Help", "Help ~Index", "~Help on Help", "~About...",
    "<- Ante", "<- Bet", "Fold", "Progressive", "Auto", "Advice", "Add $100",
    "Cash Out", "Exit", "Help",
    "Bank: $", "Jackpot: $", "Player paid $", "Progressive pays $",
    "Caribbean Stud", "Caribbean Draw",
    "Fold", "Bet", "Bet or fold.", "Click up to two cards to discard them, then bet.",
    "Nothing", "Ace/King", "Two Pair", "Flush", "Full House", "Royal Flush",
    "Pair of %s", "Three %s", "Four %s", "Straight (%s high)", "Straight Flush (%s high)",
    "Cannot load deck library!", "Error!", "You cash out $%.2f. Thanks for playing!",
    "Your bank cannot cover this ante.\nChoose Cash - Add $100.",
    "Caribbean Poker Help", "The help file %s was not found.\nPlease put it in the help folder next to CPoker.exe."
},

/* ======================= LANG_ES ======================= */
{
    "~Juego", "~Nuevo juego\tCtrl+N", "~Stud", "~Draw", "~Salir\tCtrl+X",
    "~Caja", "Anadir ~$100", "~Retirarse",
    "~Opciones", "~Sonido", "~Activado", "~Desactivado",
    "~Idioma", "~Controles de marco\tCtrl+F", "~Guardar al salir",
    "A~yuda", "~Ayuda general", "~Indice de ayuda", "Ayuda so~bre la ayuda", "~Acerca de...",
    "<- Ante", "<- Apostar", "Retirar", "Progresivo", "Auto", "Consejo", "Anadir $100",
    "Retirarse", "Salir", "Ayuda",
    "Banco: $", "Bote: $", "Jugador cobra $", "El progresivo paga $",
    "Caribbean Stud", "Caribbean Draw",
    "Retirar", "Apostar", "Apostar o retirar.", "Pulse hasta dos cartas para descartarlas y apueste.",
    "Nada", "As/Rey", "Doble pareja", "Color", "Full", "Escalera real",
    "Pareja de %s", "Trio de %s", "Poker de %s", "Escalera (carta alta %s)", "Escalera de color (carta alta %s)",
    "No se puede cargar la baraja!", "Error!", "Se retira con $%.2f. Gracias por jugar!",
    "Su banco no cubre este ante.\nElija Caja - Anadir $100.",
    "Ayuda de Caribbean Poker", "No se encontro el archivo de ayuda %s.\nPongalo en la carpeta help junto a CPoker.exe."
},

/* ======================= LANG_NL ======================= */
{
    "~Spel", "~Nieuw spel\tCtrl+N", "~Stud", "~Draw", "~Afsluiten\tCtrl+X",
    "~Kas", "$100 ~toevoegen", "~Uitbetalen",
    "~Opties", "~Geluid", "~Aan", "~Uit",
    "~Taal", "~Kaderbediening\tCtrl+F", "Instellingen op~slaan bij afsluiten",
    "~Help", "~Algemene help", "Help~index", "~Help bij help", "~Info...",
    "<- Inzet", "<- Inzetten", "Passen", "Progressief", "Auto", "Advies", "$100 erbij",
    "Uitbetalen", "Einde", "Help",
    "Bank: $", "Jackpot: $", "Speler krijgt $", "Progressief betaalt $",
    "Caribbean Stud", "Caribbean Draw",
    "Passen", "Inzetten", "Inzetten of passen.", "Klik op maximaal twee kaarten om ze weg te gooien en zet in.",
    "Niets", "Aas/Heer", "Twee paar", "Flush", "Full House", "Royal Flush",
    "Paar %s", "Drie %s", "Vier %s", "Straat (%s hoog)", "Straight Flush (%s hoog)",
    "Kan de kaartenset niet laden!", "Fout!", "U neemt $%.2f mee. Bedankt voor het spelen!",
    "Uw bank dekt deze inzet niet.\nKies Kas - $100 toevoegen.",
    "Caribbean Poker Help", "Het helpbestand %s is niet gevonden.\nPlaats het in de map help naast CPoker.exe."
},

/* ======================= LANG_DE ======================= */
{
    "~Spiel", "~Neues Spiel\tCtrl+N", "~Stud", "~Draw", "~Beenden\tCtrl+X",
    "~Kasse", "$100 ~hinzufuegen", "~Auszahlen",
    "~Optionen", "~Ton", "~Ein", "~Aus",
    "~Sprache", "~Rahmenelemente\tCtrl+F", "Beim Beenden Einstellungen ~sichern",
    "~Hilfe", "~Allgemeine Hilfe", "Hilfe~index", "~Hilfe zur Hilfe", "~Ueber...",
    "<- Einsatz", "<- Setzen", "Passen", "Progressiv", "Auto", "Rat", "$100 dazu",
    "Auszahlen", "Ende", "Hilfe",
    "Bank: $", "Jackpot: $", "Spieler erhaelt $", "Progressiv zahlt $",
    "Caribbean Stud", "Caribbean Draw",
    "Passen", "Setzen", "Setzen oder passen.", "Bis zu zwei Karten anklicken, um sie abzulegen, dann setzen.",
    "Nichts", "Ass/Koenig", "Zwei Paare", "Flush", "Full House", "Royal Flush",
    "Paar %s", "Drilling %s", "Vierling %s", "Strasse (%s hoch)", "Straight Flush (%s hoch)",
    "Kartensatz kann nicht geladen werden!", "Fehler!", "Sie nehmen $%.2f mit. Danke fuers Spielen!",
    "Ihre Bank deckt diesen Einsatz nicht.\nWaehlen Sie Kasse - $100 hinzufuegen.",
    "Caribbean Poker Hilfe", "Die Hilfedatei %s wurde nicht gefunden.\nBitte in den Ordner help neben CPoker.exe legen."
},

/* ======================= LANG_FR ======================= */
{
    "~Jeu", "~Nouveau jeu\tCtrl+N", "~Stud", "~Draw", "~Sortir\tCtrl+X",
    "~Caisse", "Ajouter ~$100", "~Encaisser",
    "~Options", "~Son", "~Actif", "~Inactif",
    "~Langue", "~Controles du cadre\tCtrl+F", "Enregistrer en ~quittant",
    "~Aide", "Aide ~generale", "~Index de l'aide", "Aide sur l'~aide", "A ~propos...",
    "<- Mise", "<- Miser", "Passer", "Progressif", "Auto", "Conseil", "Ajouter $100",
    "Encaisser", "Sortir", "Aide",
    "Banque: $", "Jackpot: $", "Joueur paye $", "Le progressif paie $",
    "Caribbean Stud", "Caribbean Draw",
    "Passer", "Miser", "Miser ou passer.", "Cliquez sur deux cartes au plus pour les jeter, puis misez.",
    "Rien", "As/Roi", "Double paire", "Couleur", "Full", "Quinte flush royale",
    "Paire de %s", "Brelan de %s", "Carre de %s", "Quinte (%s haut)", "Quinte flush (%s haut)",
    "Impossible de charger le jeu de cartes!", "Erreur!", "Vous encaissez $%.2f. Merci d'avoir joue!",
    "Votre banque ne couvre pas cette mise.\nChoisissez Caisse - Ajouter $100.",
    "Aide de Caribbean Poker", "Le fichier d'aide %s est introuvable.\nPlacez-le dans le dossier help a cote de CPoker.exe."
},

/* ======================= LANG_IT ======================= */
{
    "~Gioco", "~Nuovo gioco\tCtrl+N", "~Stud", "~Draw", "~Esci\tCtrl+X",
    "~Cassa", "Aggiungi ~$100", "~Incassa",
    "~Opzioni", "~Suono", "~Attivo", "~Disattivo",
    "~Lingua", "~Controlli cornice\tCtrl+F", "Salva impostazioni all'~uscita",
    "~Aiuto", "Aiuto ~generale", "~Indice dell'aiuto", "Aiuto sull'~aiuto", "~Informazioni...",
    "<- Ante", "<- Punta", "Passa", "Progressivo", "Auto", "Consiglio", "Aggiungi $100",
    "Incassa", "Esci", "Aiuto",
    "Banco: $", "Jackpot: $", "Giocatore paga $", "Il progressivo paga $",
    "Caribbean Stud", "Caribbean Draw",
    "Passa", "Punta", "Punta o passa.", "Fai clic su al massimo due carte per scartarle, poi punta.",
    "Niente", "Asso/Re", "Doppia coppia", "Colore", "Full", "Scala reale",
    "Coppia di %s", "Tris di %s", "Poker di %s", "Scala (%s alto)", "Scala colore (%s alto)",
    "Impossibile caricare il mazzo!", "Errore!", "Incassi $%.2f. Grazie per aver giocato!",
    "Il tuo banco non copre questo ante.\nScegli Cassa - Aggiungi $100.",
    "Aiuto di Caribbean Poker", "Il file di aiuto %s non e stato trovato.\nMetterlo nella cartella help accanto a CPoker.exe."
}
};
