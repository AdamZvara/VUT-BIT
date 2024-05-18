using System;
using RideWithMe.App.Exceptions;
using RideWithMe.BL.Models;

namespace RideWithMe.App.Services;

public class LoggedInUser : ILoggedInUser
{
    private Guid Id { get; set; } = Guid.Empty;

    private static string _tryGetExeptionMessage = "Trying to access logged user ID before logging user!";
    private static string _tryLogEmptyUser = "Trying to login not existing user!";
    public Guid GetLoggedUserGuid()
    {
        return Id == Guid.Empty ? throw new NotSingInUserException(_tryGetExeptionMessage) : Id;
    }
    public void SetLoggedUserGuid(Guid newLoggedUser)
    {
        if (newLoggedUser == Guid.Empty)
            throw new NotSingInUserException(_tryLogEmptyUser);
        Id = newLoggedUser;
    }


}