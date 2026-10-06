/* Caribbean Stud Poker - language table. ASCII only (no accents).
   Order of the strings must match the enum in cslang.h. */

#include <os2.h>
#include "cslang.h"

int current_lang = LANG_EN;

const char *lang_strings[LANG_COUNT][S_COUNT] = {

/* ======================= LANG_EN ======================= */
{
    "~Game", "~New Game\tCtrl+N", "Add ~$100", "~Cash Out", "S~tatistics...\tS", "E~xit\tCtrl+X",
    "~Options", "~Sound", "~On", "O~ff",
    "~Language", "~Frame Controls\tCtrl+F", "S~ave settings on exit",
    "~Help", "~General Help", "Help ~Index", "~Help on Help", "~About...",
    "<- Ante", "<- Bet", "Fold", "<- Progressive", "Auto", "Add $100", "Advice",
    "Cash Out", "Stats", "Exit", "Help",
    "Dealer has:", "Player Paid:", "Progressive pays:", "Bank $", "Jackpot",
    "Fold", "Bet",
    "Nothing", "Ace/King", "Pair", "Two Pair", "Three of a kind", "Straight",
    "Flush", "Full House", "Four of a kind", "Straight Flush", "Royal Flush",
    "Cannot load deck library!", "Error!", "Are you sure you want to reset all statistics?",
    "Warning!", "You cash out $%.2f. Thanks for playing!",
    "Your bank cannot cover this ante.\nChoose Game - Add $100.",
    "Caribbean Stud Help", "The help file %s was not found.\nPlease put it in the help folder next to CStud.exe.",
    "Game Statistics", "Player Hands", "Outcomes", "Money", "Total", "Win",
    "Lose", "Fold", "Dealer fold", "Money In", "Money Out",
    "Progressives", "Prog. Payout", "Reset", "OK"
},

/* ======================= LANG_ES ======================= */
{
    "~Juego", "~Nuevo juego\tCtrl+N", "Anadir ~$100", "~Retirarse", "~Estadisticas...\tS", "~Salir\tCtrl+X",
    "~Opciones", "~Sonido", "~Activado", "~Desactivado",
    "~Idioma", "~Controles de marco\tCtrl+F", "~Guardar al salir",
    "A~yuda", "~Ayuda general", "~Indice de ayuda", "Ayuda so~bre la ayuda", "~Acerca de...",
    "<- Ante", "<- Apostar", "Retirar", "<- Progresivo", "Auto", "Anadir $100", "Consejo",
    "Retirarse", "Estad.", "Salir", "Ayuda",
    "La banca tiene:", "Jugador cobra:", "El progresivo paga:", "Banco $", "Bote",
    "Retirar", "Apostar",
    "Nada", "As/Rey", "Pareja", "Doble pareja", "Trio", "Escalera",
    "Color", "Full", "Poker", "Escalera de color", "Escalera real",
    "No se puede cargar la baraja!", "Error!", "Seguro que quiere borrar todas las estadisticas?",
    "Atencion!", "Se retira con $%.2f. Gracias por jugar!",
    "Su banco no cubre este ante.\nElija Juego - Anadir $100.",
    "Ayuda de Caribbean Stud", "No se encontro el archivo de ayuda %s.\nPongalo en la carpeta help junto a CStud.exe.",
    "Estadisticas del juego", "Manos del jugador", "Resultados", "Dinero", "Total", "Gana",
    "Pierde", "Retira", "Banca no abre", "Dinero entra", "Dinero sale",
    "Progresivos", "Pago progr.", "Reiniciar", "Aceptar"
},

/* ======================= LANG_NL ======================= */
{
    "~Spel", "~Nieuw spel\tCtrl+N", "$100 ~toevoegen", "~Uitbetalen", "S~tatistieken...\tS", "~Afsluiten\tCtrl+X",
    "~Opties", "~Geluid", "~Aan", "~Uit",
    "~Taal", "~Kaderbediening\tCtrl+F", "Instellingen op~slaan bij afsluiten",
    "~Help", "~Algemene help", "Help~index", "~Help bij help", "~Info...",
    "<- Inzet", "<- Inzetten", "Passen", "<- Progressief", "Auto", "$100 erbij", "Advies",
    "Uitbetalen", "Stats", "Einde", "Help",
    "Deler heeft:", "Speler krijgt:", "Progressief betaalt:", "Bank $", "Jackpot",
    "Passen", "Inzetten",
    "Niets", "Aas/Heer", "Paar", "Twee paar", "Drie gelijke", "Straat",
    "Flush", "Full House", "Vier gelijke", "Straight Flush", "Royal Flush",
    "Kan de kaartenset niet laden!", "Fout!", "Weet u zeker dat u alle statistieken wilt wissen?",
    "Waarschuwing!", "U neemt $%.2f mee. Bedankt voor het spelen!",
    "Uw bank dekt deze inzet niet.\nKies Spel - $100 toevoegen.",
    "Caribbean Stud Help", "Het helpbestand %s is niet gevonden.\nPlaats het in de map help naast CStud.exe.",
    "Spelstatistieken", "Handen van de speler", "Uitkomsten", "Geld", "Totaal", "Winst",
    "Verlies", "Gepast", "Deler past", "Geld erin", "Geld eruit",
    "Progressieven", "Progr. uitbet.", "Wissen", "OK"
},

/* ======================= LANG_DE ======================= */
{
    "~Spiel", "~Neues Spiel\tCtrl+N", "$100 ~hinzufuegen", "~Auszahlen", "S~tatistik...\tS", "~Beenden\tCtrl+X",
    "~Optionen", "~Ton", "~Ein", "~Aus",
    "~Sprache", "~Rahmenelemente\tCtrl+F", "Beim Beenden Einstellungen ~sichern",
    "~Hilfe", "~Allgemeine Hilfe", "Hilfe~index", "~Hilfe zur Hilfe", "~Ueber...",
    "<- Einsatz", "<- Setzen", "Passen", "<- Progressiv", "Auto", "$100 dazu", "Rat",
    "Auszahlen", "Statistik", "Ende", "Hilfe",
    "Geber hat:", "Spieler erhaelt:", "Progressiv zahlt:", "Bank $", "Jackpot",
    "Passen", "Setzen",
    "Nichts", "Ass/Koenig", "Paar", "Zwei Paare", "Drilling", "Strasse",
    "Flush", "Full House", "Vierling", "Straight Flush", "Royal Flush",
    "Kartensatz kann nicht geladen werden!", "Fehler!", "Wollen Sie wirklich die gesamte Statistik zuruecksetzen?",
    "Warnung!", "Sie nehmen $%.2f mit. Danke fuers Spielen!",
    "Ihre Bank deckt diesen Einsatz nicht.\nWaehlen Sie Spiel - $100 hinzufuegen.",
    "Caribbean Stud Hilfe", "Die Hilfedatei %s wurde nicht gefunden.\nBitte in den Ordner help neben CStud.exe legen.",
    "Spielstatistik", "Haende des Spielers", "Ergebnisse", "Geld", "Gesamt", "Gewonnen",
    "Verloren", "Gepasst", "Geber passt", "Geld rein", "Geld raus",
    "Progressive", "Progr. Auszahlung", "Zuruecksetzen", "OK"
},

/* ======================= LANG_FR ======================= */
{
    "~Jeu", "~Nouveau jeu\tCtrl+N", "Ajouter ~$100", "~Encaisser", "~Statistiques...\tS", "~Sortir\tCtrl+X",
    "~Options", "~Son", "~Actif", "~Inactif",
    "~Langue", "~Controles du cadre\tCtrl+F", "Enregistrer en ~quittant",
    "~Aide", "Aide ~generale", "~Index de l'aide", "Aide sur l'~aide", "A ~propos...",
    "<- Mise", "<- Miser", "Passer", "<- Progressif", "Auto", "Ajouter $100", "Conseil",
    "Encaisser", "Stats", "Sortir", "Aide",
    "Le croupier a:", "Joueur paye:", "Le progressif paie:", "Banque $", "Jackpot",
    "Passer", "Miser",
    "Rien", "As/Roi", "Paire", "Double paire", "Brelan", "Quinte",
    "Couleur", "Full", "Carre", "Quinte flush", "Quinte flush royale",
    "Impossible de charger le jeu de cartes!", "Erreur!", "Voulez-vous vraiment effacer toutes les statistiques?",
    "Attention!", "Vous encaissez $%.2f. Merci d'avoir joue!",
    "Votre banque ne couvre pas cette mise.\nChoisissez Jeu - Ajouter $100.",
    "Aide de Caribbean Stud", "Le fichier d'aide %s est introuvable.\nPlacez-le dans le dossier help a cote de CStud.exe.",
    "Statistiques du jeu", "Mains du joueur", "Resultats", "Argent", "Total", "Gains",
    "Pertes", "Passe", "Croupier passe", "Argent entre", "Argent sorti",
    "Progressifs", "Gain progr.", "Effacer", "OK"
},

/* ======================= LANG_IT ======================= */
{
    "~Gioco", "~Nuovo gioco\tCtrl+N", "Aggiungi ~$100", "~Incassa", "~Statistiche...\tS", "~Esci\tCtrl+X",
    "~Opzioni", "~Suono", "~Attivo", "~Disattivo",
    "~Lingua", "~Controlli cornice\tCtrl+F", "Salva impostazioni all'~uscita",
    "~Aiuto", "Aiuto ~generale", "~Indice dell'aiuto", "Aiuto sull'~aiuto", "~Informazioni...",
    "<- Ante", "<- Punta", "Passa", "<- Progressivo", "Auto", "Aggiungi $100", "Consiglio",
    "Incassa", "Stat.", "Esci", "Aiuto",
    "Il banco ha:", "Giocatore paga:", "Il progressivo paga:", "Banco $", "Jackpot",
    "Passa", "Punta",
    "Niente", "Asso/Re", "Coppia", "Doppia coppia", "Tris", "Scala",
    "Colore", "Full", "Poker", "Scala colore", "Scala reale",
    "Impossibile caricare il mazzo!", "Errore!", "Sicuro di voler azzerare tutte le statistiche?",
    "Attenzione!", "Incassi $%.2f. Grazie per aver giocato!",
    "Il tuo banco non copre questo ante.\nScegli Gioco - Aggiungi $100.",
    "Aiuto di Caribbean Stud", "Il file di aiuto %s non e stato trovato.\nMetterlo nella cartella help accanto a CStud.exe.",
    "Statistiche di gioco", "Mani del giocatore", "Risultati", "Denaro", "Totale", "Vinte",
    "Perse", "Passate", "Banco passa", "Denaro in", "Denaro out",
    "Progressivi", "Pag. progr.", "Azzera", "OK"
}
};
