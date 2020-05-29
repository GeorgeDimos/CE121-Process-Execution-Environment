#Εργασία 4: Περιβάλλον ελεγχόμενης εκτέλεσης προγραμμάτων μέσω σημάτων

###### Εαρινό εξάμηνο 2017-2018

Υλοποιήστε ένα «περιβάλλον» εκτέλεσης προγραμμάτων το οποίο να υποστηρίζει τις εξής εντολές χρήστη:

|Εντολή|Ορίσματα|Περιγραφή|
|------|--------|---------|
exec | \<progname><args\> | Εκτέλεση του προγράμματος progname με ορίσματα args, μέσω ξεχωριστής διεργασίας (fork-exec).
term|\<pid\>|Αποστολή σήματος SIGTERM στη διεργασία με αναγνωριστικό pid.
sig|\<pid\>|Αποστολή σήματος SIGUSR1 στη διεργασία με αναγνωριστικό pid.
list||Πληροφορίες για τις διεργασίες που εξακολουθούν να υφίστανται: αναγνωριστικό, όνομα προγράμματος, ορίσματα, κατάσταση (αν τρέχει).
quit||Τερματισμός περιβάλλοντος εκτέλεσης.

Με την εντολή exec αρχίζει η εκτέλεση ενός προγράμματος μέσω μιας νέας διεργασίας που δημιουργεί το
περιβάλλον για αυτό τον σκοπό μέσω fork-exec. Τα αναγνωριστικά των διεργασιών πρέπει να διατηρούνται σε ένα
πίνακα ή λίστα που ανανεώνεται όταν δημιουργείται / τερματίζεται μια διεργασία (σημείωση: μια διεργασία μπορεί να
τερματίσει μόνη της ή λόγω εξωτερικών παραγόντων, χωρίς ο χρήστης να δώσει την εντολή term ).

Τα προγράμματα που ζητά ο χρήστης να εκτελεστούν μέσα από το περιβάλλον εκτέλεσης μπορεί να είναι οτιδήποτε
(για πρακτικούς λόγους καλό είναι να μην διαβάζουν από την συμβατική είσοδο και να μην εκτυπώνουν πολλή
πληροφορία στην συμβατική έξοδο τους). Επίσης, θα χρειαστεί να υλοποιήσετε εσείς ένα πρόγραμμα που ικανοποιεί
συγκεκριμένες προδιαγραφές (βλ. επόμενη σελίδα).

Πριν την εκκίνηση μιας διεργασίας που εκτελεί πρόγραμμα που ζήτησε ο χρήστης, το περιβάλλον εκτέλεσης πρέπει να
αποκαθιστά το συμβατικό χειρισμό του σήματος SIGUSR1 γι αυτή τη διεργασία. Επίσης, το ίδιο το περιβάλλον
χειρίζεται το SIGUSR1 προωθώντας το σε όσες διεργασίες-παιδιά είναι ακόμα ενεργές (δεν έχουν ήδη τερματιστεί).

Με την εντολή quit , το περιβάλλον πρέπει να ελευθερώνει τη δυναμικά δεσμευμένη μνήμη του, να σκοτώνει όλες τις
διεργασίες-παιδιά που είναι ακόμη ενεργές και να μην αφήνει ζόμπι (να περιμένει τον τερματισμό τους).

Παράλληλα με την υποστήριξη των εντολών χρήστη, το περιβάλλον πρέπει να λειτουργεί και ως ένας απλός
χρονοδρομολογητής έτσι ώστε κάθε στιγμή να εκτελείται μόνο μία διεργασία-παιδί (αν υπάρχει), και κάθε διεργασία
να εκτελείται για συγκεκριμένο μερίδιο χρόνου. Η επιλογή της επόμενης διεργασίας είναι κυκλική (round-robin). Η
εναλλαγή διεργασιών πρέπει να γίνεται στέλνοντας σήματα SIGSTOP και SIGCONT στις αντίστοιχες διεργασίες, ενώ
η εκπνοή του χρόνου της τρέχουσας διεργασίας πρέπει να ελέγχεται μέσω ενός «ξυπνητηριού» σε συνδυασμό με το
σήμα SIGALRM. Επίσης, εναλλαγή πρέπει να γίνεται και όταν τερματίζεται η διεργασία που εκτελείται, ακόμα και αν
δεν έχει εκπνεύσει ο χρόνος της.
