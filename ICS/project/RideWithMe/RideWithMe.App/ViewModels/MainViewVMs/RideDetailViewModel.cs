using System;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Input;
using RideWithMe.App.Commands;
using RideWithMe.App.Messages;
using RideWithMe.App.Messages.ViewMessages;
using RideWithMe.App.Services;
using RideWithMe.App.ViewModels.Interfaces;
using RideWithMe.App.Views;
using RideWithMe.App.Wrappers;
using RideWithMe.BL.Facades;
using RideWithMe.BL.Models;

namespace RideWithMe.App.ViewModels.MainViewVMs;

public class RideDetailViewModel : ViewModelBase, IRideDetailViewModel
{
    private readonly RideFacade _rideFacade;
    private readonly IMediator _mediator;
    private readonly UserFacade _userFacade;
    private readonly ILoggedInUser _loggedInUser;

    public RideDetailViewModel(
        RideFacade rideFacade,
        UserFacade userFacade,
        ILoggedInUser loggedInUser,
        IMediator mediator)
    {
        _rideFacade = rideFacade;
        _userFacade = userFacade;
        _mediator = mediator;
        _loggedInUser = loggedInUser;

        CloseViewCommand = new RelayCommand(OnCloseView);
        JoinRideCommand = new AsyncRelayCommand(AddLoggedInUserToRide, CanAddToRide);
        LeaveRideCommand = new AsyncRelayCommand(DeleteLoggedInUserFromRide, CanRemoveFromRide);
        CancelRideCommand = new AsyncRelayCommand(CancelRide, IsDriver);
        RemovePassengerCommand = new AsyncRelayCommand<string>(RemovePassengerFromRide);


        // Register mediator message to get Guid of selected ride
        _mediator.Register<SelectedMessage<RideWrapper>>(OnRideSelected);
        _mediator.Register<RefreshMessage<RideWrapper>>(OnAppRefresh);
    }

    public RideWrapper? Model { get; private set; }

    public ICommand CloseViewCommand { get; }

    public AsyncRelayCommand JoinRideCommand { get; }
    public AsyncRelayCommand LeaveRideCommand { get; }
    public AsyncRelayCommand CancelRideCommand { get; }
    public AsyncRelayCommand<string> RemovePassengerCommand { get; }
    public Guid SelectedRideGuid { get; private set; }
    public Guid SelectedRideDriverGuid { get; private set; }
    public bool IsDriverPermissions { get; set; }

    public void OnCloseView()
    {
        _mediator.Send(new CloseRideDetailMessage<RideWrapper>());
        _mediator.Send(new UnselectMessage<RideWrapper>());
    } 

    public void OnRideSelected(SelectedMessage<RideWrapper> message)
    {
        SelectedRideGuid = message.Id ?? Guid.Empty;
        CancelRideCommand.NotifyCanExecuteChanged();
    }
    private async void OnAppRefresh(RefreshMessage<RideWrapper> _)
    {
        if (SelectedRideGuid != Guid.Empty)
            await LoadAsync(SelectedRideGuid);
    }

    public async Task LoadAsync(Guid id)
    {
        Model = await _rideFacade.GetAsync(id) ?? RideDetailModel.Empty;
        if (Model.Driver != null)
            IsDriverPermissions = Model!.Driver!.Id == _loggedInUser.GetLoggedUserGuid();
    }

    public async Task AddLoggedInUserToRide()
    {
        var user = await _userFacade.GetAsync(_loggedInUser.GetLoggedUserGuid());
        var ride = await _rideFacade.GetAsync(SelectedRideGuid);
        await _rideFacade.AddPassengerToRide(ride!, user!);

        await LoadAsync(SelectedRideGuid);

        JoinRideCommand.NotifyCanExecuteChanged();
        LeaveRideCommand.NotifyCanExecuteChanged();
    }

    public async Task DeleteLoggedInUserFromRide()
    {
        var user = await _userFacade.GetAsync(_loggedInUser.GetLoggedUserGuid());
        var ride = await _rideFacade.GetAsync(SelectedRideGuid);
        await _rideFacade.RemovePassengerFromRide(ride!, user!);

        await LoadAsync(SelectedRideGuid);

        JoinRideCommand.NotifyCanExecuteChanged();
        LeaveRideCommand.NotifyCanExecuteChanged();
    }

    public async Task DeleteAllPassengers()
    {
        foreach (var passenger in Model!.Passengers)
        {
            await _rideFacade.RemovePassengerFromRide(Model.Model, passenger);
        }
    }

    public async Task CancelRide()
    {
        try
        {
            await DeleteAllPassengers();
            await _rideFacade.DeleteAsync(Model!.Id);
        }
        catch
        {
            Console.WriteLine("Delete failed");
            throw;
        }

        CancelRideCommand.NotifyCanExecuteChanged();
        _mediator.Send(new UpdateMessage<RideWrapper>());
        _mediator.Send(new CloseRideDetailMessage<RideWrapper>());
    }

    private async Task RemovePassengerFromRide(string? Firstname)
    {
        foreach (var passenger in Model!.Passengers)
        {
            if (passenger.FirstName == Firstname)
                await _rideFacade.RemovePassengerFromRide(Model!.Model, passenger);
        }

        await LoadAsync(Model.Id);
    }

    private bool CanSave() => Model?.IsValid ?? false;

    private bool CanAddToRide()
    {
        Guid loggedId;
        try
        {
            loggedId = _loggedInUser.GetLoggedUserGuid();
        }
        catch
        {
            return false;
        }

        // + 2 because driver isn't in Passengers list ( Passengers.Count + new person + Driver <= Car.Seats
        return (loggedId != Model?.Driver?.Id) && (!(Model?.Passengers.Any(user => user.Id == loggedId)) ?? false) &&
               (Model?.Passengers.Count + 2 <= Model?.Car?.Seats);
    }

    private bool CanRemoveFromRide()
    {
        Guid loggedId;
        try
        {
            loggedId = _loggedInUser.GetLoggedUserGuid();
        }
        catch
        {
            return false;
        }

        return Model?.Passengers.Any(user => user.Id == loggedId) ?? false;
    }

    private bool IsDriver()
    {
        Guid loggedId;
        try
        {
            loggedId = _loggedInUser.GetLoggedUserGuid();
        }
        catch
        {
            return false;
        }
        if (Model != null && Model.Driver != null)
            return Model?.Driver!.Id == loggedId;
        else
            return false;
    }

    public async Task DeleteAsync()
    {
    }

    public async Task SaveAsync()
    {
    }
}