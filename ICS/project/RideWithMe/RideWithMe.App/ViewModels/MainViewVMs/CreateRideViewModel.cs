using System;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Windows.Input;
using Microsoft.EntityFrameworkCore.Internal;
using Microsoft.Toolkit.Mvvm.Input;
using RideWithMe.App.Enums;
using RideWithMe.App.Messages;
using RideWithMe.App.Messages.ViewMessages;
using RideWithMe.App.Services;
using RideWithMe.App.ViewModels.Interfaces;
using RideWithMe.App.Views.MainViewViews;
using RideWithMe.App.Wrappers;
using RideWithMe.BL.Facades;
using RideWithMe.BL.Models;
using RideWithMe.Common.Enums;
using RideWithMe.DAL.Seeds;

namespace RideWithMe.App.ViewModels.MainViewVMs;

public class CreateRideViewModel : ViewModelBase, ICreateRideViewModel
{
    private readonly IMediator _mediator;
    private readonly RideFacade _rideFacade;
    private readonly ILoggedInUser _loggedInUser;

    public CreateRideViewModel(
        RideFacade rideFacade,
        IMediator mediator,
        IAddressListViewModel startAddressListViewModel,
        IAddressListViewModel endAddressListViewModel,
        ICarListViewModel carListViewModel,
        ILoggedInUser loggedInUser)
    {
        StartAddressListViewModel = startAddressListViewModel;
        StartAddressListViewModel.SetType(LocationTypes.StartLocation);


        EndAddressListViewModel = endAddressListViewModel;
        EndAddressListViewModel.SetType(LocationTypes.EndLocation);


        CarListViewModel = carListViewModel;
        _mediator = mediator;
        _rideFacade = rideFacade;
        _loggedInUser = loggedInUser;

        mediator.Register<SelectedAddressMessage<AddressWrapper>>(OnAddressSelected);
        mediator.Register<SelectedMessage<CarWrapper>>(OnCarSelected);

        SaveCommand = new AsyncRelayCommand(SaveAsync, CanSave);
        CloseCreateRideView = new RelayCommand(CloseCreateRideMessage);
        OpenAddressDetailList = new RelayCommand(OnOpenAddressDetailList);

        ClearModel();
    }

    public IAddressListViewModel StartAddressListViewModel { get; }
    public IAddressListViewModel EndAddressListViewModel { get; }
    public ICarListViewModel CarListViewModel { get; }
    private void OnAddressSelected(SelectedAddressMessage<AddressWrapper> addressMessage)
    {
        var locationId = addressMessage.Id ?? Guid.Empty;
        if(addressMessage.LocationType == LocationTypes.StartLocation)
            Model.StartLocationId = locationId;
        else if (addressMessage.LocationType == LocationTypes.EndLocation)
            Model.EndLocationId = locationId;
        else if (addressMessage.LocationType == LocationTypes.None)
            throw new InvalidOperationException("Address view model has no type initialized!");
    }

    private void OnCarSelected(SelectedMessage<CarWrapper> carMessage)
    {
        Model.CarId = carMessage.Id;
    }

    public RideWrapper Model { get; private set; } 

    private void ClearModel()
    {
        Model = RideDetailModel.Empty;
        Model.SetCallOnSave(SaveCommand.NotifyCanExecuteChanged);
    }

    public AsyncRelayCommand SaveCommand { get; }
    public ICommand CloseCreateRideView { get; }
    public ICommand OpenAddressDetailList { get; }

    private void OnOpenAddressDetailList()
        => _mediator.Send(new OpenAddressDetailMessage<AddressWrapper>());

    private void CloseCreateRideMessage()
    {
        _mediator.Send(new UpdateMessage<CarWrapper>());
        _mediator.Send(new CloseCreateRideDetailMessage<RideWrapper>());
        _mediator.Send(new RefreshMessage<AddressWrapper>());
        ClearModel();
    }

    public async Task LoadAsync(Guid id)
    {
        Model = await _rideFacade.GetAsync(id) ?? RideDetailModel.Empty;
        ClearModel();
    }

    public async Task SaveAsync()
    {
        if (Model == null)
        {
            throw new InvalidOperationException("Null model cannot be saved");
        }

        Model.DriverId = _loggedInUser.GetLoggedUserGuid();
        await _rideFacade.SaveAsync(Model.Model);

        _mediator.Send(new UpdateMessage<RideWrapper> { Model = Model });
        _mediator.Send(new RefreshMessage<AddressWrapper>());
        CloseCreateRideMessage();
    }

    private bool CanSave() => Model?.IsValid ?? false;
    public async Task DeleteAsync(){}

}