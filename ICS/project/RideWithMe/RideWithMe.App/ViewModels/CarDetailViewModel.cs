using System;
using System.Threading.Tasks;
using System.Windows.Input;
using Microsoft.EntityFrameworkCore.Metadata.Internal;
using Microsoft.Toolkit.Mvvm.Input;
using RideWithMe.App.Messages;
using RideWithMe.App.Messages.ViewMessages;
using RideWithMe.App.Services;
using RideWithMe.App.Services.MessageDialog;
using RideWithMe.App.ViewModels.Interfaces;
using RideWithMe.App.Wrappers;
using RideWithMe.BL.Facades;
using RideWithMe.BL.Models;

namespace RideWithMe.App.ViewModels;

public class CarDetailViewModel : ViewModelBase, ICarDetailViewModel
{
    private readonly IMediator _mediator;
    private readonly IMessageDialogService _messageDialogService;
    private readonly CarFacade _carFacade;
    private readonly ILoggedInUser _loggedInUser;

    public CarDetailViewModel(
        CarFacade carFacade,
        ILoggedInUser loggedInUser,
        IMediator mediator,
        IMessageDialogService messageDialogService)
    {
        _mediator = mediator;
        _messageDialogService = messageDialogService;
        _carFacade = carFacade;
        _loggedInUser = loggedInUser;

        SaveCommand = new AsyncRelayCommand(SaveAsync, CanSave);
        DeleteCommand = new AsyncRelayCommand(DeleteAsync);
        CloseCarDetailViewCommand = new RelayCommand(CloseDetailView);
    }

    public CarWrapper? Model { get; private set; }
    public AsyncRelayCommand SaveCommand { get; }
    public ICommand DeleteCommand { get; }
    public ICommand CloseCarDetailViewCommand { get; }

    private void CloseDetailView() => _mediator.Send(new CloseCarDetailMessage<CarWrapper>());

    public async Task LoadAsync(Guid id)
    {
        Model = await _carFacade.GetAsync(id) ?? CarDetailModel.Empty;
        Model.SetCallOnSave(SaveCommand.NotifyCanExecuteChanged);
    }

    public async Task SaveAsync()
    {
        if (Model == null)
        {
            throw new InvalidOperationException("Cant save null car");
        }

        Model.OwnerId = _loggedInUser.GetLoggedUserGuid();
        Model = await _carFacade.SaveAsync(Model.Model);
        //_mediator.Send(new UpdateMessage<CarWrapper> { Model = Model });
        _mediator.Send(new CloseCarDetailMessage<CarWrapper>());
    }

    private bool CanSave() => Model?.IsValid ?? false;

    public async Task DeleteAsync()
    {
        if (Model == null)
        {
            throw new InvalidOperationException("Cant delete null car");
        }
        try
        {
            await _carFacade.DeleteAsync(Model.Model);
            _mediator.Send(new CloseCarDetailMessage<CarWrapper>());
        }
        catch
        {
            ShowAddressIsBeingUsed();
        }

    }

    private void ShowAddressIsBeingUsed()
    {
        _messageDialogService.Show(
            $"Car delete error",
            $"Cannot delete car which is being used.",
            MessageDialogButtonConfiguration.OK,
            MessageDialogResult.No);
    }
}   