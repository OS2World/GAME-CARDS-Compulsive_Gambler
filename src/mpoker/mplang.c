/* Multi Poker - language table. ASCII only (no accents).
   Order of the strings must match the enum in mplang.h. */

#include <os2.h>
#include "mplang.h"

int current_lang = LANG_EN;

const char *lang_strings[LANG_COUNT][S_COUNT] = {

/* ======================= LANG_EN ======================= */
{
    "~Game", "~New Game\tCtrl+N", "~Select", "E~xit\tCtrl+X",
    "~Options", "~Sound", "~On", "O~ff",
    "~Language", "~Frame Controls\tCtrl+F", "S~ave settings on exit",
    "~Help", "~General Help", "Help ~Index", "~Help on Help", "~About...",
    "~Deal", "Bet", "Bet Five", "~Advice", "~Odds", "Keep/Discard",
    "Keep", "Discard", "Bank: $", "Bet", "Hand",
    "No game libraries were found in the games folder.", "Cannot load the card deck of this game.", "Error!",
    "You are out of money.\nChoose Game - New Game to get $25.",
    "Multi Poker Help", "The help file %s was not found.\nPlease put it in the help folder next to MPoker.exe.",
    "Odds", "Expected payout", "This game does not calculate odds.", "Deal a hand first."
},

/* ======================= LANG_ES ======================= */
{
    "~Juego", "~Nuevo juego\tCtrl+N", "~Elegir", "~Salir\tCtrl+X",
    "~Opciones", "~Sonido", "~Activado", "~Desactivado",
    "~Idioma", "~Controles de marco\tCtrl+F", "~Guardar al salir",
    "A~yuda", "~Ayuda general", "~Indice de ayuda", "Ayuda so~bre la ayuda", "~Acerca de...",
    "~Repartir", "Apostar", "Apostar 5", "~Consejo", "~Probabilidades", "Guardar/Descartar",
    "Guardar", "Descartar", "Banco: $", "Apuesta", "Mano",
    "No se encontraron juegos en la carpeta games.", "No se puede cargar la baraja de este juego.", "Error!",
    "Se ha quedado sin dinero.\nElija Juego - Nuevo juego para obtener $25.",
    "Ayuda de Multi Poker", "No se encontro el archivo de ayuda %s.\nPongalo en la carpeta help junto a MPoker.exe.",
    "Probabilidades", "Pago esperado", "Este juego no calcula probabilidades.", "Reparta primero una mano."
},

/* ======================= LANG_NL ======================= */
{
    "~Spel", "~Nieuw spel\tCtrl+N", "~Kiezen", "~Afsluiten\tCtrl+X",
    "~Opties", "~Geluid", "~Aan", "~Uit",
    "~Taal", "~Kaderbediening\tCtrl+F", "Instellingen op~slaan bij afsluiten",
    "~Help", "~Algemene help", "Help~index", "~Help bij help", "~Info...",
    "~Delen", "Inzetten", "Vijf inzetten", "~Advies", "~Kansen", "Houden/Weggooien",
    "Houden", "Weggooien", "Bank: $", "Inzet", "Hand",
    "Geen spelbibliotheken gevonden in de map games.", "Kan de kaartenset van dit spel niet laden.", "Fout!",
    "U hebt geen geld meer.\nKies Spel - Nieuw spel voor $25.",
    "Multi Poker Help", "Het helpbestand %s is niet gevonden.\nPlaats het in de map help naast MPoker.exe.",
    "Kansen", "Verwachte uitbetaling", "Dit spel berekent geen kansen.", "Deel eerst een hand."
},

/* ======================= LANG_DE ======================= */
{
    "~Spiel", "~Neues Spiel\tCtrl+N", "~Waehlen", "~Beenden\tCtrl+X",
    "~Optionen", "~Ton", "~Ein", "~Aus",
    "~Sprache", "~Rahmenelemente\tCtrl+F", "Beim Beenden Einstellungen ~sichern",
    "~Hilfe", "~Allgemeine Hilfe", "Hilfe~index", "~Hilfe zur Hilfe", "~Ueber...",
    "~Geben", "Setzen", "Fuenf setzen", "~Rat", "~Chancen", "Behalten/Ablegen",
    "Behalten", "Ablegen", "Bank: $", "Einsatz", "Hand",
    "Im Ordner games wurden keine Spielbibliotheken gefunden.", "Der Kartensatz dieses Spiels kann nicht geladen werden.", "Fehler!",
    "Sie haben kein Geld mehr.\nWaehlen Sie Spiel - Neues Spiel fuer $25.",
    "Multi Poker Hilfe", "Die Hilfedatei %s wurde nicht gefunden.\nBitte in den Ordner help neben MPoker.exe legen.",
    "Chancen", "Erwartete Auszahlung", "Dieses Spiel berechnet keine Chancen.", "Geben Sie zuerst eine Hand."
},

/* ======================= LANG_FR ======================= */
{
    "~Jeu", "~Nouveau jeu\tCtrl+N", "~Choisir", "~Sortir\tCtrl+X",
    "~Options", "~Son", "~Actif", "~Inactif",
    "~Langue", "~Controles du cadre\tCtrl+F", "Enregistrer en ~quittant",
    "~Aide", "Aide ~generale", "~Index de l'aide", "Aide sur l'~aide", "A ~propos...",
    "~Donner", "Miser", "Miser cinq", "~Conseil", "~Chances", "Garder/Jeter",
    "Garder", "Jeter", "Banque: $", "Mise", "Main",
    "Aucune bibliotheque de jeu dans le dossier games.", "Impossible de charger le jeu de cartes de ce jeu.", "Erreur!",
    "Vous n'avez plus d'argent.\nChoisissez Jeu - Nouveau jeu pour $25.",
    "Aide de Multi Poker", "Le fichier d'aide %s est introuvable.\nPlacez-le dans le dossier help a cote de MPoker.exe.",
    "Chances", "Gain attendu", "Ce jeu ne calcule pas les chances.", "Distribuez d'abord une main."
},

/* ======================= LANG_IT ======================= */
{
    "~Gioco", "~Nuovo gioco\tCtrl+N", "~Scegli", "~Esci\tCtrl+X",
    "~Opzioni", "~Suono", "~Attivo", "~Disattivo",
    "~Lingua", "~Controlli cornice\tCtrl+F", "Salva impostazioni all'~uscita",
    "~Aiuto", "Aiuto ~generale", "~Indice dell'aiuto", "Aiuto sull'~aiuto", "~Informazioni...",
    "~Distribuisci", "Punta", "Punta cinque", "~Consiglio", "~Probabilita", "Tieni/Scarta",
    "Tieni", "Scarta", "Banco: $", "Puntata", "Mano",
    "Nessuna libreria di gioco trovata nella cartella games.", "Impossibile caricare il mazzo di questo gioco.", "Errore!",
    "Hai finito il denaro.\nScegli Gioco - Nuovo gioco per $25.",
    "Aiuto di Multi Poker", "Il file di aiuto %s non e stato trovato.\nMetterlo nella cartella help accanto a MPoker.exe.",
    "Probabilita", "Pagamento atteso", "Questo gioco non calcola le probabilita.", "Distribuisci prima una mano."
}
};
