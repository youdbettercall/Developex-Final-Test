Dear Gentlemen,

Dear Code-Reviewer,

This is the final revision of my realization of Developex test for C++/Qt Developer.

Of course it has room for improvement, but test deadline is coming and I want to make a brave attempt to provide You what I have to this moment.

Speaking about test actually, I've made some assumptions:

1. I've decided that simple and straightforward User Interface will be enough.

2. For my convenience, I gladly held on an idea that URL can be substring that starts with "HTTP://" and ends with ".html" - to avoid difficulties with SSL and avoid parsing pdf, js, and other non-HTML URLs. 

3. UI -  I've skipped such elements as 
pause, progress bar, adaptivity and stretching of Main Window Widget - all these features I definitely can make but decided to avoid to meet the deadline.

_Details of realization_:

1. The main feature that requires improvement in my realization - is multithreading search.

For downloading and searching some text on webpages I've used QNetworkAccessManager class and QNetworkReply class. 
I know that QNetworkAccessManager can simultaneously work in 6 threads, and maybe I should have used this possibility. 
Also, I guess I should have used QThreadPool for parallel search and for setting the limit of threads.

I've tested my app up to maximum 1000 URLs. Of course, if the search was multithreaded I would be much faster -  and I'm definitely going to add this feature.

2. Also, in my opinion, validation of user input should be improved.

3. Pause/resume functionality can be implemented using condition variables, I guess.


Beg my pardon in advance for all imperfections of my code. 
I would be very grateful for any feedback and any advice.
I hope if You could evaluate my code and my professional level as adequate I would be very happy to join your team and will do my best to grow professionally.

With respect, Maxim Zhmak. 
(maximzhmak@gmail.com)
