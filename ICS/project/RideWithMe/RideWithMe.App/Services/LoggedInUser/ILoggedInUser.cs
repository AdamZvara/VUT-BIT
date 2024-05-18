using System;

namespace RideWithMe.App.Services;

public interface ILoggedInUser
{
    public void SetLoggedUserGuid(Guid newLoggedUser);
    public Guid GetLoggedUserGuid();
}