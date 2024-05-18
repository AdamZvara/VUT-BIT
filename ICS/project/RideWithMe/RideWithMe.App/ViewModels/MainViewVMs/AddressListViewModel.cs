    using System;
using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.Linq;
using System.Text;
using System.Threading.Tasks;
    using System.Windows.Input;
    using Microsoft.Toolkit.Mvvm.Input;
    using RideWithMe.App.Enums;
    using RideWithMe.App.Extensions;
    using RideWithMe.App.Messages;
    using RideWithMe.App.Services;
    using RideWithMe.App.ViewModels.Interfaces;
    using RideWithMe.App.Views.MainViewViews;
    using RideWithMe.App.Wrappers;
    using RideWithMe.BL.Facades;
    using RideWithMe.BL.Models.Address;

    namespace RideWithMe.App.ViewModels.MainViewVMs
{
    internal class AddressListViewModel : ViewModelBase, IAddressListViewModel
    {
        private readonly AddressFacade _addressFacade;
        private readonly IMediator _mediator;

        private LocationTypes Type = LocationTypes.None;
        private AddressListModel? _selectedModel;

        public AddressListViewModel(
            AddressFacade addressFacade,
            IMediator mediator)
        {
            _addressFacade = addressFacade;
            _mediator = mediator;


            _mediator.Register<RefreshMessage<AddressWrapper>>(OnRefreshAddresses);
        }

        private void OnSelectionChanged(AddressListModel? model)
        {
            _mediator.Send<SelectedAddressMessage<AddressWrapper>>(new() {Id = model?.Id, LocationType = Type});
        }

        private async void OnRefreshAddresses(RefreshMessage<AddressWrapper> _)
        {
            await LoadAsync();
        }

        public AddressListModel? SelectedModel
        {
            get => _selectedModel;
            set
            {
                _selectedModel = value; 
                OnSelectionChanged(value);
            }
        }
        
        public ObservableCollection<AddressListModel> Addresses { get; set; } = new();
        public async Task LoadAsync()
        {
            Addresses.Clear();
            var addresses = await _addressFacade.GetAllAsync();
            Addresses.AddRange(addresses);
            AddressesCount = Addresses.Count;
        }

        public void SetType(LocationTypes type) => Type = type;

        public int AddressesCount { get; private set; }
    }
}
