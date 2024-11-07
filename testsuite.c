
#include "kprintf.h"
#include "file.h"


static const char article1[] = {
    "Article. I.\n\nSection. 1.\n\nAll legislative Powers herein granted shall be"
    " vested in a Congress of the United States, which shall consist of a Sen"
    "ate and House of Representatives.\n\nSection. 2.\n\nThe House of Representat"
    "ives shall be composed of Members chosen every second Year by the People"
    " of the several States, and the Electors in each State shall have the Qu"
    "alifications requisite for Electors of the most numerous Branch of the S"
    "tate Legislature.\n\nNo Person shall be a Representative who shall not hav"
    "e attained to the Age of twenty five Years, and been seven Years a Citiz"
    "en of the United States, and who shall not, when elected, be an Inhabita"
    "nt of that State in which he shall be chosen.\n\nRepresentatives and direc"
    "t Taxes shall be apportioned among the several States which may be inclu"
    "ded within this Union, according to their respective Numbers, which shal"
    "l be determined by adding to the whole Number of free Persons, including"
    " those bound to Service for a Term of Years, and excluding Indians not t"
    "axed, three fifths of all other Persons. The actual Enumeration shall be"
    " made within three Years after the first Meeting of the Congress of the "
    "United States, and within every subsequent Term of ten Years, in such Ma"
    "nner as they shall by Law direct. The Number of Representatives shall no"
    "t exceed one for every thirty Thousand, but each State shall have at Lea"
    "st one Representative; and until such enumeration shall be made, the Sta"
    "te of New Hampshire shall be entitled to chuse three, Massachusetts eigh"
    "t, Rhode-Island and Providence Plantations one, Connecticut five, New-Yo"
    "rk six, New Jersey four, Pennsylvania eight, Delaware one, Maryland six,"
    " Virginia ten, North Carolina five, South Carolina five, and Georgia thr"
    "ee.\n\nWhen vacancies happen in the Representation from any State, the Exe"
    "cutive Authority thereof shall issue Writs of Election to fill such Vaca"
    "ncies.\n\nThe House of Representatives shall chuse their Speaker and other"
    " Officers; and shall have the sole Power of Impeachment.\n\nSection. 3.\n\nT"
    "he Senate of the United States shall be composed of two Senators from ea"
    "ch State, chosen by the Legislature thereof for six Years; and each Sena"
    "tor shall have one Vote.\n\nImmediately after they shall be assembled in C"
    "onsequence of the first Election, they shall be divided as equally as ma"
    "y be into three Classes. The Seats of the Senators of the first Class sh"
    "all be vacated at the Expiration of the second Year, of the second Class"
    " at the Expiration of the fourth Year, and of the third Class at the Exp"
    "iration of the sixth Year, so that one third may be chosen every second "
    "Year; and if Vacancies happen by Resignation, or otherwise, during the R"
    "ecess of the Legislature of any State, the Executive thereof may make te"
    "mporary Appointments until the next Meeting of the Legislature, which sh"
    "all then fill such Vacancies.\n\nNo Person shall be a Senator who shall no"
    "t have attained to the Age of thirty Years, and been nine Years a Citize"
    "n of the United States, and who shall not, when elected, be an Inhabitan"
    "t of that State for which he shall be chosen.\n\nThe Vice President of the"
    " United States shall be President of the Senate, but shall have no Vote,"
    " unless they be equally divided.\n\nThe Senate shall chuse their other Off"
    "icers, and also a President pro tempore, in the Absence of the Vice Pres"
    "ident, or when he shall exercise the Office of President of the United S"
    "tates.\n\nThe Senate shall have the sole Power to try all Impeachments. Wh"
    "en sitting for that Purpose, they shall be on Oath or Affirmation. When "
    "the President of the United States is tried, the Chief Justice shall pre"
    "side: And no Person shall be convicted without the Concurrence of two th"
    "irds of the Members present.\n\nJudgment in Cases of Impeachment shall not"
    " extend further than to removal from Office, and disqualification to hol"
    "d and enjoy any Office of honor, Trust or Profit under the United States"
    ": but the Party convicted shall nevertheless be liable and subject to In"
    "dictment, Trial, Judgment and Punishment, according to Law.\n\nSection. 4."
    "\n\nThe Times, Places and Manner of holding Elections for Senators and Rep"
    "resentatives, shall be prescribed in each State by the Legislature there"
    "of; but the Congress may at any time by Law make or alter such Regulatio"
    "ns, except as to the Places of chusing Senators.\n\nThe Congress shall ass"
    "emble at least once in every Year, and such Meeting shall be on the firs"
    "t Monday in December, unless they shall by Law appoint a different Day.\n"
    "\nSection. 5.\n\nEach House shall be the Judge of the Elections, Returns an"
    "d Qualifications of its own Members, and a Majority of each shall consti"
    "tute a Quorum to do Business; but a smaller Number may adjourn from day "
    "to day, and may be authorized to compel the Attendance of absent Members"
    ", in such Manner, and under such Penalties as each House may provide.\n\nE"
    "ach House may determine the Rules of its Proceedings, punish its Members"
    " for disorderly Behaviour, and, with the Concurrence of two thirds, expe"
    "l a Member.\n\nEach House shall keep a Journal of its Proceedings, and fro"
    "m time to time publish the same, excepting such Parts as may in their Ju"
    "dgment require Secrecy; and the Yeas and Nays of the Members of either H"
    "ouse on any question shall, at the Desire of one fifth of those Present,"
    " be entered on the Journal.\n\nNeither House, during the Session of Congre"
    "ss, shall, without the Consent of the other, adjourn for more than three"
    " days, nor to any other Place than that in which the two Houses shall be"
    " sitting.\n\nSection. 6.\n\nThe Senators and Representatives shall receive a"
    " Compensation for their Services, to be ascertained by Law, and paid out"
    " of the Treasury of the United States. They shall in all Cases, except T"
    "reason, Felony and Breach of the Peace, be privileged from Arrest during"
    " their Attendance at the Session of their respective Houses, and in goin"
    "g to and returning from the same; and for any Speech or Debate in either"
    " House, they shall not be questioned in any other Place.\n\nNo Senator or "
    "Representative shall, during the Time for which he was elected, be appoi"
    "nted to any civil Office under the Authority of the United States, which"
    " shall have been created, or the Emoluments whereof shall have been encr"
    "eased during such time; and no Person holding any Office under the Unite"
    "d States, shall be a Member of either House during his Continuance in Of"
    "fice.\n\nSection. 7.\n\nAll Bills for raising Revenue shall originate in the"
    " House of Representatives; but the Senate may propose or concur with Ame"
    "ndments as on other Bills.\n\nEvery Bill which shall have passed the House"
    " of Representatives and the Senate, shall, before it become a Law, be pr"
    "esented to the President of the United States: If he approve he shall si"
    "gn it, but if not he shall return it, with his Objections to that House "
    "in which it shall have originated, who shall enter the Objections at lar"
    "ge on their Journal, and proceed to reconsider it. If after such Reconsi"
    "deration two thirds of that House shall agree to pass the Bill, it shall"
    " be sent, together with the Objections, to the other House, by which it "
    "shall likewise be reconsidered, and if approved by two thirds of that Ho"
    "use, it shall become a Law. But in all such Cases the Votes of both Hous"
    "es shall be determined by yeas and Nays, and the Names of the Persons vo"
    "ting for and against the Bill shall be entered on the Journal of each Ho"
    "use respectively. If any Bill shall not be returned by the President wit"
    "hin ten Days (Sundays excepted) after it shall have been presented to hi"
    "m, the Same shall be a Law, in like Manner as if he had signed it, unles"
    "s the Congress by their Adjournment prevent its Return, in which Case it"
    " shall not be a Law.\n\nEvery Order, Resolution, or Vote to which the Conc"
    "urrence of the Senate and House of Representatives may be necessary (exc"
    "ept on a question of Adjournment) shall be presented to the President of"
    " the United States; and before the Same shall take Effect, shall be appr"
    "oved by him, or being disapproved by him, shall be repassed by two third"
    "s of the Senate and House of Representatives, according to the Rules and"
    " Limitations prescribed in the Case of a Bill.\n\nSection. 8.\n\nThe Congres"
    "s shall have Power To lay and collect Taxes, Duties, Imposts and Excises"
    ", to pay the Debts and provide for the common Defence and general Welfar"
    "e of the United States; but all Duties, Imposts and Excises shall be uni"
    "form throughout the United States;\n\nTo borrow Money on the credit of the"
    " United States;\n\nTo regulate Commerce with foreign Nations, and among th"
    "e several States, and with the Indian Tribes;\n\nTo establish an uniform R"
    "ule of Naturalization, and uniform Laws on the subject of Bankruptcies t"
    "hroughout the United States;\n\nTo coin Money, regulate the Value thereof,"
    " and of foreign Coin, and fix the Standard of Weights and Measures;\n\nTo "
    "provide for the Punishment of counterfeiting the Securities and current "
    "Coin of the United States;\n\nTo establish Post Offices and post Roads;\n\nT"
    "o promote the Progress of Science and useful Arts, by securing for limit"
    "ed Times to Authors and Inventors the exclusive Right to their respectiv"
    "e Writings and Discoveries;\n\nTo constitute Tribunals inferior to the sup"
    "reme Court;\n\nTo define and punish Piracies and Felonies committed on the"
    " high Seas, and Offences against the Law of Nations;\n\nTo declare War, gr"
    "ant Letters of Marque and Reprisal, and make Rules concerning Captures o"
    "n Land and Water;\n\nTo raise and support Armies, but no Appropriation of "
    "Money to that Use shall be for a longer Term than two Years;\n\nTo provide"
    " and maintain a Navy;\n\nTo make Rules for the Government and Regulation o"
    "f the land and naval Forces;\n\nTo provide for calling forth the Militia t"
    "o execute the Laws of the Union, suppress Insurrections and repel Invasi"
    "ons;\n\nTo provide for organizing, arming, and disciplining, the Militia, "
    "and for governing such Part of them as may be employed in the Service of"
    " the United States, reserving to the States respectively, the Appointmen"
    "t of the Officers, and the Authority of training the Militia according t"
    "o the discipline prescribed by Congress;\n\nTo exercise exclusive Legislat"
    "ion in all Cases whatsoever, over such District (not exceeding ten Miles"
    " square) as may, by Cession of particular States, and the Acceptance of "
    "Congress, become the Seat of the Government of the United States, and to"
    " exercise like Authority over all Places purchased by the Consent of the"
    " Legislature of the State in which the Same shall be, for the Erection o"
    "f Forts, Magazines, Arsenals, dock-Yards, and other needful Buildings;--"
    "And\n\nTo make all Laws which shall be necessary and proper for carrying i"
    "nto Execution the foregoing Powers, and all other Powers vested by this "
    "Constitution in the Government of the United States, or in any Departmen"
    "t or Officer thereof.\n\nSection. 9.\n\nThe Migration or Importation of such"
    " Persons as any of the States now existing shall think proper to admit, "
    "shall not be prohibited by the Congress prior to the Year one thousand e"
    "ight hundred and eight, but a Tax or duty may be imposed on such Importa"
    "tion, not exceeding ten dollars for each Person.\n\nThe Privilege of the W"
    "rit of Habeas Corpus shall not be suspended, unless when in Cases of Reb"
    "ellion or Invasion the public Safety may require it.\n\nNo Bill of Attaind"
    "er or ex post facto Law shall be passed.\n\nNo Capitation, or other direct"
    ", Tax shall be laid, unless in Proportion to the Census or enumeration h"
    "erein before directed to be taken.\n\nNo Tax or Duty shall be laid on Arti"
    "cles exported from any State.\n\nNo Preference shall be given by any Regul"
    "ation of Commerce or Revenue to the Ports of one State over those of ano"
    "ther; nor shall Vessels bound to, or from, one State, be obliged to ente"
    "r, clear, or pay Duties in another.\n\nNo Money shall be drawn from the Tr"
    "easury, but in Consequence of Appropriations made by Law; and a regular "
    "Statement and Account of the Receipts and Expenditures of all public Mon"
    "ey shall be published from time to time.\n\nNo Title of Nobility shall be "
    "granted by the United States: And no Person holding any Office of Profit"
    " or Trust under them, shall, without the Consent of the Congress, accept"
    " of any present, Emolument, Office, or Title, of any kind whatever, from"
    " any King, Prince, or foreign State.\n\nSection. 10.\n\nNo State shall enter"
    " into any Treaty, Alliance, or Confederation; grant Letters of Marque an"
    "d Reprisal; coin Money; emit Bills of Credit; make any Thing but gold an"
    "d silver Coin a Tender in Payment of Debts; pass any Bill of Attainder, "
    "ex post facto Law, or Law impairing the Obligation of Contracts, or gran"
    "t any Title of Nobility.\n\nNo State shall, without the Consent of the Con"
    "gress, lay any Imposts or Duties on Imports or Exports, except what may "
    "be absolutely necessary for executing it's inspection Laws: and the net "
    "Produce of all Duties and Imposts, laid by any State on Imports or Expor"
    "ts, shall be for the Use of the Treasury of the United States; and all s"
    "uch Laws shall be subject to the Revision and Controul of the Congress.\n"
    "\nNo State shall, without the Consent of Congress, lay any Duty of Tonnag"
    "e, keep Troops, or Ships of War in time of Peace, enter into any Agreeme"
    "nt or Compact with another State, or with a foreign Power, or engage in "
    "War, unless actually invaded, or in such imminent Danger as will not adm"
    "it of delay."
};

static const char article2[] = {
    "Article. II.\n\nSection. 1.\n\nThe executive Power shall be vested in a Pres"
    "ident of the United States of America. He shall hold his Office during t"
    "he Term of four Years, and, together with the Vice President, chosen for"
    " the same Term, be elected, as follows:\n\nEach State shall appoint, in su"
    "ch Manner as the Legislature thereof may direct, a Number of Electors, e"
    "qual to the whole Number of Senators and Representatives to which the St"
    "ate may be entitled in the Congress: but no Senator or Representative, o"
    "r Person holding an Office of Trust or Profit under the United States, s"
    "hall be appointed an Elector.\n\nThe Electors shall meet in their respecti"
    "ve States, and vote by Ballot for two Persons, of whom one at least shal"
    "l not be an Inhabitant of the same State with themselves. And they shall"
    " make a List of all the Persons voted for, and of the Number of Votes fo"
    "r each; which List they shall sign and certify, and transmit sealed to t"
    "he Seat of the Government of the United States, directed to the Presiden"
    "t of the Senate. The President of the Senate shall, in the Presence of t"
    "he Senate and House of Representatives, open all the Certificates, and t"
    "he Votes shall then be counted. The Person having the greatest Number of"
    " Votes shall be the President, if such Number be a Majority of the whole"
    " Number of Electors appointed; and if there be more than one who have su"
    "ch Majority, and have an equal Number of Votes, then the House of Repres"
    "entatives shall immediately chuse by Ballot one of them for President; a"
    "nd if no Person have a Majority, then from the five highest on the List "
    "the said House shall in like Manner chuse the President. But in chusing "
    "the President, the Votes shall be taken by States, the Representation fr"
    "om each State having one Vote; A quorum for this purpose shall consist o"
    "f a Member or Members from two thirds of the States, and a Majority of a"
    "ll the States shall be necessary to a Choice. In every Case, after the C"
    "hoice of the President, the Person having the greatest Number of Votes o"
    "f the Electors shall be the Vice President. But if there should remain t"
    "wo or more who have equal Votes, the Senate shall chuse from them by Bal"
    "lot the Vice President.\n\nThe Congress may determine the Time of chusing "
    "the Electors, and the Day on which they shall give their Votes; which Da"
    "y shall be the same throughout the United States.\n\nNo Person except a na"
    "tural born Citizen, or a Citizen of the United States, at the time of th"
    "e Adoption of this Constitution, shall be eligible to the Office of Pres"
    "ident; neither shall any Person be eligible to that Office who shall not"
    " have attained to the Age of thirty five Years, and been fourteen Years "
    "a Resident within the United States.\n\nIn Case of the Removal of the Pres"
    "ident from Office, or of his Death, Resignation, or Inability to dischar"
    "ge the Powers and Duties of the said Office, the Same shall devolve on t"
    "he Vice President, and the Congress may by Law provide for the Case of R"
    "emoval, Death, Resignation or Inability, both of the President and Vice "
    "President, declaring what Officer shall then act as President, and such "
    "Officer shall act accordingly, until the Disability be removed, or a Pre"
    "sident shall be elected.\n\nThe President shall, at stated Times, receive "
    "for his Services, a Compensation, which shall neither be increased nor d"
    "iminished during the Period for which he shall have been elected, and he"
    " shall not receive within that Period any other Emolument from the Unite"
    "d States, or any of them.\n\nBefore he enter on the Execution of his Offic"
    "e, he shall take the following Oath or Affirmation:--\"I do solemnly swea"
    "r (or affirm) that I will faithfully execute the Office of President of "
    "the United States, and will to the best of my Ability, preserve, protect"
    " and defend the Constitution of the United States.\"\n\nSection. 2.\n\nThe Pr"
    "esident shall be Commander in Chief of the Army and Navy of the United S"
    "tates, and of the Militia of the several States, when called into the ac"
    "tual Service of the United States; he may require the Opinion, in writin"
    "g, of the principal Officer in each of the executive Departments, upon a"
    "ny Subject relating to the Duties of their respective Offices, and he sh"
    "all have Power to grant Reprieves and Pardons for Offences against the U"
    "nited States, except in Cases of Impeachment.\n\nHe shall have Power, by a"
    "nd with the Advice and Consent of the Senate, to make Treaties, provided"
    " two thirds of the Senators present concur; and he shall nominate, and b"
    "y and with the Advice and Consent of the Senate, shall appoint Ambassado"
    "rs, other public Ministers and Consuls, Judges of the supreme Court, and"
    " all other Officers of the United States, whose Appointments are not her"
    "ein otherwise provided for, and which shall be established by Law: but t"
    "he Congress may by Law vest the Appointment of such inferior Officers, a"
    "s they think proper, in the President alone, in the Courts of Law, or in"
    " the Heads of Departments.\n\nThe President shall have Power to fill up al"
    "l Vacancies that may happen during the Recess of the Senate, by granting"
    " Commissions which shall expire at the End of their next Session.\n\nSecti"
    "on. 3.\n\nHe shall from time to time give to the Congress Information of t"
    "he State of the Union, and recommend to their Consideration such Measure"
    "s as he shall judge necessary and expedient; he may, on extraordinary Oc"
    "casions, convene both Houses, or either of them, and in Case of Disagree"
    "ment between them, with Respect to the Time of Adjournment, he may adjou"
    "rn them to such Time as he shall think proper; he shall receive Ambassad"
    "ors and other public Ministers; he shall take Care that the Laws be fait"
    "hfully executed, and shall Commission all the Officers of the United Sta"
    "tes.\n\nSection. 4.\n\nThe President, Vice President and all civil Officers "
    "of the United States, shall be removed from Office on Impeachment for, a"
    "nd Conviction of, Treason, Bribery, or other high Crimes and Misdemeanor"
    "s."
};

static const char article3[] = {
    "Article III.\n\nSection. 1.\n\nThe judicial Power of the United States shall"
    " be vested in one supreme Court, and in such inferior Courts as the Cong"
    "ress may from time to time ordain and establish. The Judges, both of the"
    " supreme and inferior Courts, shall hold their Offices during good Behav"
    "iour, and shall, at stated Times, receive for their Services a Compensat"
    "ion, which shall not be diminished during their Continuance in Office.\n\n"
    "Section. 2.\n\nThe judicial Power shall extend to all Cases, in Law and Eq"
    "uity, arising under this Constitution, the Laws of the United States, an"
    "d Treaties made, or which shall be made, under their Authority;--to all "
    "Cases affecting Ambassadors, other public Ministers and Consuls;--to all"
    " Cases of admiralty and maritime Jurisdiction;--to Controversies to whic"
    "h the United States shall be a Party;--to Controversies between two or m"
    "ore States;-- between a State and Citizens of another State,--between Ci"
    "tizens of different States,--between Citizens of the same State claiming"
    " Lands under Grants of different States, and between a State, or the Cit"
    "izens thereof, and foreign States, Citizens or Subjects.\n\nIn all Cases a"
    "ffecting Ambassadors, other public Ministers and Consuls, and those in w"
    "hich a State shall be Party, the supreme Court shall have original Juris"
    "diction. In all the other Cases before mentioned, the supreme Court shal"
    "l have appellate Jurisdiction, both as to Law and Fact, with such Except"
    "ions, and under such Regulations as the Congress shall make.\n\nThe Trial "
    "of all Crimes, except in Cases of Impeachment, shall be by Jury; and suc"
    "h Trial shall be held in the State where the said Crimes shall have been"
    " committed; but when not committed within any State, the Trial shall be "
    "at such Place or Places as the Congress may by Law have directed.\n\nSecti"
    "on. 3.\n\nTreason against the United States, shall consist only in levying"
    " War against them, or in adhering to their Enemies, giving them Aid and "
    "Comfort. No Person shall be convicted of Treason unless on the Testimony"
    " of two Witnesses to the same overt Act, or on Confession in open Court."
};

static const char article4[] = {
    "Article. IV.\n\nSection. 1.\n\nFull Faith and Credit shall be given in each "
    "State to the public Acts, Records, and judicial Proceedings of every oth"
    "er State. And the Congress may by general Laws prescribe the Manner in w"
    "hich such Acts, Records and Proceedings shall be proved, and the Effect "
    "thereof.\n\nSection. 2.\n\nThe Citizens of each State shall be entitled to a"
    "ll Privileges and Immunities of Citizens in the several States.\n\nA Perso"
    "n charged in any State with Treason, Felony, or other Crime, who shall f"
    "lee from Justice, and be found in another State, shall on Demand of the "
    "executive Authority of the State from which he fled, be delivered up, to"
    " be removed to the State having Jurisdiction of the Crime.\n\nNo Person he"
    "ld to Service or Labour in one State, under the Laws thereof, escaping i"
    "nto another, shall, in Consequence of any Law or Regulation therein, be "
    "discharged from such Service or Labour, but shall be delivered up on Cla"
    "im of the Party to whom such Service or Labour may be due.\n\nSection. 3.\n"
    "\nNew States may be admitted by the Congress into this Union; but no new "
    "State shall be formed or erected within the Jurisdiction of any other St"
    "ate; nor any State be formed by the Junction of two or more States, or P"
    "arts of States, without the Consent of the Legislatures of the States co"
    "ncerned as well as of the Congress.\n\nThe Congress shall have Power to di"
    "spose of and make all needful Rules and Regulations respecting the Terri"
    "tory or other Property belonging to the United States; and nothing in th"
    "is Constitution shall be so construed as to Prejudice any Claims of the "
    "United States, or of any particular State.\n\nSection. 4.\n\nThe United Stat"
    "es shall guarantee to every State in this Union a Republican Form of Gov"
    "ernment, and shall protect each of them against Invasion; and on Applica"
    "tion of the Legislature, or of the Executive (when the Legislature canno"
    "t be convened), against domestic Violence."
};

static const char article5[] = {
    "Article. V.\n\nThe Congress, whenever two thirds of both Houses shall deem"
    " it necessary, shall propose Amendments to this Constitution, or, on the"
    " Application of the Legislatures of two thirds of the several States, sh"
    "all call a Convention for proposing Amendments, which, in either Case, s"
    "hall be valid to all Intents and Purposes, as Part of this Constitution,"
    " when ratified by the Legislatures of three fourths of the several State"
    "s, or by Conventions in three fourths thereof, as the one or the other M"
    "ode of Ratification may be proposed by the Congress; Provided that no Am"
    "endment which may be made prior to the Year One thousand eight hundred a"
    "nd eight shall in any Manner affect the first and fourth Clauses in the "
    "Ninth Section of the first Article; and that no State, without its Conse"
    "nt, shall be deprived of its equal Suffrage in the Senate."
};

static const char article6[] = {
    "Article. VI.\n\n"
    "All Debts contracted and Engagements entered into, before the Adoption "
    "of this Constitution, shall be as valid against the United States under "
    "this Constitution, as under the Confederation.\n\n"
    "This Constitution, and the Laws of the United States which shall be made "
    "in Pursuance thereof; and all Treaties made, or which shall be made, "
    "under the Authority of the United States, shall be the supreme Law of the "
    "Land; and the Judges in every State shall be bound thereby, any Thing in the "
    "Constitution or Laws of any State to the Contrary notwithstanding.\n\n"
    "The Senators and Representatives before mentioned, and the Members of "
    "the several State Legislatures, and all executive and judicial Officers, "
    "both of the United States and of the several States, shall be bound by Oath or "
    "Affirmation, to support this Constitution; but no religious Test shall ever "
    "be required as a Qualification to any Office or public Trust under the United States." };



static unsigned mystrlen(const char* s){
    unsigned len=0;
    while(*s){
        len++;
        s++;
    }
    return len;
}


static void freeze(){
    while(1){
        __asm__("cli");
        __asm__("hlt");
    }
}

static int fds[6];
static const char* article_content[] = {article1, article2,article3,article4,article5,article6};
static const char* filenames[6] = { "article1.txt","article2.txt","article3.txt","article4.txt","article5.txt","article6.txt"};
static unsigned lengths[6];
static int numLeftToOpen=6;

static char article1_[14050];
static char article2_[6050];
static char article3_[2150];
static char article4_[2050];
static char article5_[950];
static char article6_[1050];

static int QUANTUM=8;


static void dumpq(const char* b, int bufferlength)
{
    for(int j=0;j<QUANTUM ;++j){
        if( j >= bufferlength )
            kprintf("   ");
        else {
            char c = b[j];
            kprintf("%02x ", (unsigned)(c) & 0xff );
        }
    }
    kprintf(" | ");
    for(int j=0;j<QUANTUM ;++j){
        if( j >= bufferlength ){
            kprintf(" ");
        } else {
            char c = b[j];
            if( c >= 32 && c <= 126 ){
                kprintf("%c", c);
            } else {
                kprintf(".");
            }
        }
    }
    return;
}

static void dumpComparison(const char* b1, const char* b2, int offset, int bufferlength){

    b1+=offset;
    if(b2)
        b2+=offset;

    while(bufferlength > 0 ){
        kprintf("%05d: ",offset);
        dumpq(b1,bufferlength);
        if( b2 ){
            kprintf(" || ");
            dumpq(b2,bufferlength);
        }
        kprintf("\n");

        b1 += QUANTUM;
        if(b2)
            b2 += QUANTUM;
        bufferlength -= QUANTUM;
        offset += QUANTUM;
    }
}


static void compareFiles(const char* buffer1, const char* buffer2, int length, const char* name)
{
    for(int i=0;i<length;i+=QUANTUM){
        for(int j=0;j<QUANTUM && i+j < length;++j){
            if( buffer1[i+j] != buffer2[i+j] ){

                int start = i - 30;
                if(start<0)
                    start=0;
                int end=i+30;
                if( end > length )
                    end=length;
                dumpComparison(buffer1,buffer2,start,end-start);
                kprintf("\ntestsuite line %d: Wrong contents in %s at offset %d\n",__LINE__, name,i+j);
                freeze();
            }
        }
    }
}


static void f1(int fd, void* d);

void sweet(){

    for(int i=0;i<6;++i){
        lengths[i] = mystrlen(article_content[i]);
    }

    for(int i=0;i<6;++i){
        kprintf("testsuite line %d: Opening %s\n",__LINE__,filenames[i]);
        file_open(filenames[i],0,f1,(void*)i);
    }
    return;
}

static void doNextRead();

static void f1(int fd, void* d){
    int i = (int)d;
    fds[i]=fd;

    if(fds[i] < 0 ){
        kprintf("testsuite line %d: Could not open %s (error %d)\n",__LINE__,filenames[i],fds[i]);
        freeze();
    }

    numLeftToOpen--;
    if( numLeftToOpen == 0 ){
        kprintf("testsuite line %d: Beginning reads...\n",__LINE__);
        doNextRead();
    }
}

static char* p[6] = {article1_, article2_, article3_, article4_, article5_, article6_ };
static unsigned count[6]={0,0,0,0,0,0};
static char canary = 0x42;
static unsigned readsize=27;
static int order[6]={0,1,2,3,4,5};
static int atEOF[6];
static int readsLeft;
static void f2(int ecode, void* buf, unsigned nr, void* x);

static void doNextRead(){


    int tmp = order[0];
    for(int i=0;i<5;++i){
        order[i]=order[i+1];
    }
    order[5]=tmp;

    readsLeft=6;
    for(int j=0;j<6;++j){
        int i = order[j];
        p[i][count[i]+readsize+1] = canary;
        file_read(fds[i],p[i]+count[i],readsize,f2,(void*)i);
    }
}

static void f2(int ecode, void* buf, unsigned nr, void* x){
    int i = (int) x;
    if( ecode ){
        kprintf("testsuite: Line %d: Error in read (%d)\n", __LINE__, ecode );
        freeze();
    }

    if( nr > readsize ){
        kprintf("testsuite: Line %d: Too much data read for %s: Got %d, expected %d or less\n",
            __LINE__,filenames[i],nr,readsize);
        freeze();
    }

    count[i] += nr;
    if( count[i] > lengths[i] ){
        kprintf("testsuite: Line %d: Did not reach EOF for %s\n",__LINE__,filenames[i]);
        freeze();
    }

    if( nr == 0 ){
        if( !atEOF[i] ){
            kprintf("testsuite: Line %d: Reached end of %s\n", __LINE__,filenames[i]);
            atEOF[i]=1;
        }
    }

    --readsLeft;
    if( readsLeft != 0 )
        return;

    if( atEOF[0] + atEOF[1] + atEOF[2] + atEOF[3] + atEOF[4] + atEOF[5] < 6 ){
        doNextRead();
        return;
    }

    //if we get here, all the reads are done
    for(int i=0;i<6;++i){
        if( count[i] != lengths[i] ){
            kprintf("testsuite line %d: Incorrect data for %s: Got %d, expected %d\n",
                __LINE__,filenames[i], count[i], lengths[i] );
            return;
        }

        compareFiles( p[i], article_content[i], lengths[i], filenames[i] );
    }


    kprintf("\n\n");

    //!!
    kprintf("All OK\n");
    //!!

}
