using System;
using System.Threading.Tasks;
using System.Windows.Input;
using Microsoft.Toolkit.Mvvm.Input;
using RideWithMe.App.Enums;
using RideWithMe.App.Messages;
using RideWithMe.App.Messages.ViewMessages;
using RideWithMe.App.Services;
using RideWithMe.App.Services.MessageDialog;
using RideWithMe.App.ViewModels.Interfaces;
using RideWithMe.App.Wrappers;
using RideWithMe.BL.Facades;
using RideWithMe.BL.Models.Address;

namespace RideWithMe.App.ViewModels;

public class AddressDetailViewModel : ViewModelBase, IAddressDetailViewModel
{
    public IAddressListViewModel AddressListViewModel { get; }
    private readonly IMediator _mediator;
    private readonly IMessageDialogService _messageDialogService;
    private readonly AddressFacade _addressFacade;
    public AddressWrapper? Model { get; set; }
    public ICommand CloseDetail { get; set; }
    public ICommand NewAddress { get; set; }
    public AsyncRelayCommand RemoveAddress { get; set; }
    public AsyncRelayCommand SaveAddress { get; set; }

    public AddressDetailViewModel(
        IMediator mediator,
        IAddressListViewModel addressListViewModel,
        IMessageDialogService messageDialogService,
        AddressFacade addressFacade)
    {
        _mediator = mediator;
        _messageDialogService = messageDialogService;
        _addressFacade = addressFacade;

        AddressListViewModel = addressListViewModel;
        AddressListViewModel.SetType(LocationTypes.Location);

        _mediator.Register<SelectedAddressMessage<AddressWrapper>>(OnAddressSelected);

        CloseDetail = new RelayCommand(OnCloseDetail);
        NewAddress = new RelayCommand(ClearAddress);
        RemoveAddress = new AsyncRelayCommand(DeleteAsync);
        SaveAddress = new AsyncRelayCommand(SaveAsync, CanSave);

        ClearAddress();
    }

    private void OnCloseDetail()
    {
        _mediator.Send(new CloseAddressDetailMessage<AddressWrapper>());
    }

    private async void OnAddressSelected(SelectedAddressMessage<AddressWrapper> msgSelectedAddressMessage)
    {
        await LoadAsync(msgSelectedAddressMessage.Id ?? Guid.Empty);
    }
    
    private async void ClearAddress()
    {
        Model = AddressDetailModel.Empty;
        await LoadAsync(Guid.Empty);
    }


    public async Task LoadAsync(Guid id)
    {
        Model = await _addressFacade.GetAsync(id) ?? AddressDetailModel.Empty;
        Model.SetCallOnSave(SaveAddress.NotifyCanExecuteChanged);
    }

    public async Task DeleteAsync()
    {
        try
        {
            await _addressFacade.DeleteAsync(Model);
            ClearAddress();
            ReloadAddressList();
        }
        catch 
        {
            ShowAddressIsBeingUsed();
        }
    }

    private void ShowAddressIsBeingUsed()
    {
        _messageDialogService.Show(
            $"Address error",
            $"Cannot delete address which is being used.",
            MessageDialogButtonConfiguration.OK,
            MessageDialogResult.No);
    }

    private void ReloadAddressList()
    {
        _mediator.Send(new RefreshMessage<AddressWrapper>());
    }

    public async Task SaveAsync()
    {
        await _addressFacade.SaveAsync(Model);
        ClearAddress();
        ReloadAddressList();
    }

    private bool CanSave() => Model?.IsValid ?? false;

}

