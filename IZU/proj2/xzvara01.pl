% napi�te program �e��c� �kol dan� predik�tem u4(LIN,VOUT), kde LIN je
% vstupn� ��seln� seznam s nejm�n� dv�ma prvky a VOUT je v�stupn�
% prom�nn�, ve kter� se vrac� hodnota rozd�lu mezi nejv�t��m a nejmen��m
% ��slem seznamu LIN.

u4_1:- u4([5,27,-1,0,16,-4],VOUT),write(VOUT).                      % 31
u4_2:- u4([5,27,-1,0,16,-24],VOUT),write(VOUT).                 % 51
u4_3:- u4([5.5,27.2],VOUT),write(VOUT).                         % 21.7
u4_r:- write('Zadej LIN: '),read(LIN),u4(LIN,VOUT),write(VOUT).

% vrat doposud maximalnu hodnotu
maxlist([], Return, Return).
% v pripade, ze aktualny prvok je vacsi, predaj ho ako najvacsi prvok
% inak zavolaj maxlist s aktualne najvacsim prvkom
maxlist([H|T], Max, Return):- H >  Max, maxlist(T, H, Return).
maxlist([H|T], Max, Return):- H =< Max, maxlist(T, Max, Return).
% prvy predikat, ktory sa spusti, invokuje rovnaky predikat ale s nastavenou
% doposial najvacsou hodnotou
maxlist([H|T], Return):- maxlist(T, H, Return).

% rovnake ako pre maxlist len zamena poradia predikatov pri porovnavani
minlist([], Return, Return).
minlist([H|T], Min, Return):- H >  Min, minlist(T, Min, Return).
minlist([H|T], Min, Return):- H =< Min, minlist(T, H, Return).
minlist([H|T], Return):- minlist(T, H, Return).

u4(LIN,VOUT):- maxlist(LIN, MAX), minlist(LIN, MIN), VOUT is MAX - MIN, !.