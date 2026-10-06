/* Blackjack - language table. ASCII only (no accents).
   Order of the strings must match the enum in bjlang.h. */

#include <os2.h>
#include "bjlang.h"

int current_lang = LANG_EN;

const char *lang_strings[LANG_COUNT][S_COUNT] = {

/* ======================= LANG_EN ======================= */
{
    "~Game", "~New Game\tCtrl+N", "Add ~$100", "~Cash Out", "E~xit\tCtrl+X",
    "~Options", "~Sound", "~On", "O~ff", "~Rules...",
    "~Language", "~Frame Controls\tCtrl+F", "S~ave settings on exit",
    "~Help", "~General Help", "Help ~Index", "~Help on Help", "~About...",
    "Cannot load deck library!", "Error!", "Blackjack!", "Bust!", "~Hit",
    "~Stand", "~Double", "S~plit", "Deal", "Advice", "Bank",
    "Push", "Lose", "Win", "Do you want insurance?",
    "Game Rules", "Player can resplit aces", "Dealer hits soft 17", "Double 10/11 only",
    "Number of decks: ", "OK", "Cancel",
    "You cash out $%.2f. Thanks for playing!", "Your bank cannot cover this bet.\nChoose Game - Add $100.",
    "Blackjack Help", "The help file %s was not found.\nPlease put it in the help folder next to BJack.exe."
},

/* ======================= LANG_ES ======================= */
{
    "~Juego", "~Nuevo juego\tCtrl+N", "Anadir ~$100", "~Retirarse", "~Salir\tCtrl+X",
    "~Opciones", "~Sonido", "~Activado", "~Desactivado", "~Reglas...",
    "~Idioma", "~Controles de marco\tCtrl+F", "~Guardar al salir",
    "A~yuda", "Ayuda ~general", "~Indice de ayuda", "Ayuda ~sobre ayuda", "~About...",
    "No se puede cargar la biblioteca de cartas!", "Error!", "Blackjack!", "Pasado!", "~Pedir",
    "Pla~ntarse", "~Doblar", "Di~vidir", "~Repartir", "Consejo", "Banco",
    "Empate", "Pierde", "Gana", "Desea un seguro?",
    "Reglas del juego", "El jugador puede volver a dividir ases", "El crupier pide con 17 blando", "Doblar solo con 10/11",
    "Numero de barajas: ", "Aceptar", "Cancelar",
    "Se retira con $%.2f. Gracias por jugar!", "Su banco no cubre esta apuesta.\nElija Juego - Anadir $100.",
    "Ayuda de Blackjack", "No se encontro el archivo de ayuda %s.\nColoquelo en la carpeta help junto a BJack.exe."
},

/* ======================= LANG_NL ======================= */
{
    "~Spel", "~Nieuw spel\tCtrl+N", "$100 ~toevoegen", "~Uitbetalen", "~Afsluiten\tCtrl+X",
    "~Opties", "~Geluid", "~Aan", "~Uit", "~Regels...",
    "~Taal", "Kader~bediening\tCtrl+F", "Instellingen o~pslaan bij afsluiten",
    "~Help", "Al~gemene Help", "Help-~index", "Help ~over Help", "~About...",
    "Kan de kaartenbibliotheek niet laden!", "Fout!", "Blackjack!", "Over!", "~Kaart",
    "~Blijven", "~Verdubbelen", "S~plitsen", "~Delen", "Advies", "Bank",
    "Gelijk", "Verlies", "Winst", "Wilt u een verzekering?",
    "Spelregels", "Speler mag azen opnieuw splitsen", "Deler neemt kaart bij zachte 17", "Alleen verdubbelen met 10/11",
    "Aantal sets kaarten: ", "OK", "Annuleren",
    "U neemt $%.2f op. Bedankt voor het spelen!", "Uw bank dekt deze inzet niet.\nKies Spel - $100 toevoegen.",
    "Blackjack Help", "Het helpbestand %s is niet gevonden.\nPlaats het in de map help naast BJack.exe."
},

/* ======================= LANG_DE ======================= */
{
    "~Spiel", "~Neues Spiel\tCtrl+N", "$100 ~hinzufuegen", "~Auszahlen", "Be~enden\tCtrl+X",
    "~Optionen", "~Ton", "~Ein", "~Aus", "~Regeln...",
    "~Sprache", "~Rahmenelemente\tCtrl+F", "Beim Beenden Einstellungen s~ichern",
    "~Hilfe", "Allgemeine ~Hilfe", "Hilfe~index", "Hilfe ~zur Hilfe", "~About...",
    "Kartenbibliothek kann nicht geladen werden!", "Fehler!", "Blackjack!", "Ueberkauft!", "~Karte",
    "~Halten", "~Verdoppeln", "~Teilen", "~Geben", "Rat", "Bank",
    "Unentschieden", "Verloren", "Gewonnen", "Moechten Sie eine Versicherung?",
    "Spielregeln", "Spieler darf Asse erneut teilen", "Geber zieht bei weicher 17", "Verdoppeln nur bei 10/11",
    "Anzahl der Kartensaetze: ", "OK", "Abbrechen",
    "Sie lassen sich $%.2f auszahlen. Danke fuers Spielen!", "Ihre Bank deckt diesen Einsatz nicht.\nWaehlen Sie Spiel - $100 hinzufuegen.",
    "Blackjack Hilfe", "Die Hilfedatei %s wurde nicht gefunden.\nBitte legen Sie sie in den Ordner help neben BJack.exe."
},

/* ======================= LANG_FR ======================= */
{
    "~Jeu", "~Nouveau jeu\tCtrl+N", "Ajouter ~$100", "~Encaisser", "~Sortir\tCtrl+X",
    "~Options", "~Son", "~Actif", "~Inactif", "~Regles...",
    "~Langue", "~Controles du cadre\tCtrl+F", "En~registrer en quittant",
    "~Aide", "Aide ~generale", "~Index de l'aide", "Aide ~sur l'aide", "~About...",
    "Impossible de charger la bibliotheque de cartes!", "Erreur!", "Blackjack!", "Brule!", "~Carte",
    "~Rester", "~Doubler", "~Separer", "Di~stribuer", "Conseil", "Banque",
    "Egalite", "Perdu", "Gagne", "Voulez-vous une assurance?",
    "Regles du jeu", "Le joueur peut resplitter les as", "Le croupier tire sur 17 souple", "Doubler seulement sur 10/11",
    "Nombre de jeux de cartes: ", "OK", "Annuler",
    "Vous encaissez $%.2f. Merci d'avoir joue!", "Votre banque ne couvre pas cette mise.\nChoisissez Jeu - Ajouter $100.",
    "Aide de Blackjack", "Le fichier d'aide %s est introuvable.\nPlacez-le dans le dossier help a cote de BJack.exe."
},

/* ======================= LANG_IT ======================= */
{
    "~Gioco", "~Nuovo gioco\tCtrl+N", "Aggiungi ~$100", "~Incassa", "~Esci\tCtrl+X",
    "~Opzioni", "~Suono", "~Attivo", "~Disattivo", "~Regole...",
    "~Lingua", "Controlli ~cornice\tCtrl+F", "Sal~va impostazioni all'uscita",
    "A~iuto", "Guida ~generale", "~Indice della guida", "Guida ~sulla guida", "~About...",
    "Impossibile caricare la libreria delle carte!", "Errore!", "Blackjack!", "Sballato!", "~Carta",
    "S~tai", "~Raddoppia", "~Dividi", "Dis~tribuisci", "Consiglio", "Banco",
    "Pari", "Perdi", "Vinci", "Vuoi l'assicurazione?",
    "Regole del gioco", "Il giocatore puo dividere di nuovo gli assi", "Il banco chiede con 17 morbido", "Raddoppia solo con 10/11",
    "Numero di mazzi: ", "OK", "Annulla",
    "Incassi $%.2f. Grazie per aver giocato!", "Il tuo banco non copre questa puntata.\nScegli Gioco - Aggiungi $100.",
    "Guida di Blackjack", "Il file della guida %s non e' stato trovato.\nMetterlo nella cartella help accanto a BJack.exe."
}

};
