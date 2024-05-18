using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using Microsoft.Toolkit.Mvvm.Input;
using RideWithMe.App.Factories;
using RideWithMe.App.Messages;
using RideWithMe.App.Messages.ViewMessages;
using RideWithMe.App.Services;
using RideWithMe.App.ViewModels.Interfaces;
using RideWithMe.App.Views;
using RideWithMe.App.Wrappers;
using RideWithMe.BL.Facades;
using RideWithMe.BL.Models;

namespace RideWithMe.App.ViewModels
{
    public class RideWithMeViewModel : ViewModelBase, IRideWithMeViewModel
    {
        private readonly ILoggedInUser _loggedInUserService;
        private readonly UserFacade _userFacade;
        private readonly IMediator _mediator;

        public RideWithMeViewModel(
            IFilterRidesViewModel filterRidesViewModel,
            IRideListViewModel rideListViewModel,
            IRideDetailViewModel rideDetailViewModel,
            ICreateRideViewModel createRideViewModel,
            IUserMenuViewModel userMenuViewModel,
            ILoggedInUser loggedInUserService,
            ICarDetailViewModel carDetailViewModel,
            IAddressDetailViewModel addressDetailViewModel,
            ICarListViewModel carListViewModel,
            IEditCarViewModel editCarViewModel,
            UserFacade userFacade,
            IMediator mediator)
        {
            FilterRidesViewModel = filterRidesViewModel;
            RideListViewModel = rideListViewModel;
            RideDetailViewModel = rideDetailViewModel;
            CreateRideViewModel = createRideViewModel;
            UserMenuViewModel = userMenuViewModel;
            CarDetailViewModel = carDetailViewModel;
            CarListViewModel = carListViewModel;
            EditCarViewModel = editCarViewModel;
            AddressDetailViewModel = addressDetailViewModel;

            _loggedInUserService = loggedInUserService;
            _userFacade = userFacade;
            _mediator = mediator;

            // Opening and closing user menu
            OpenUserMenuCommand = new RelayCommand(OnOpenUserMenu);
            CloseUserMenuCommand = new RelayCommand(OnCloseUserMenu);


            // Register view model visibility messages
            _mediator.Register<OpenRideWithMeViewMessage<UserWrapper>>(InitializeView);
            _mediator.Register<CloseRideDetailMessage<RideWrapper>>(CloseRideDetail);
            _mediator.Register<OpenCreateRideDetailMessage<RideWrapper>>(OpenCreateRideDetail);
            _mediator.Register<CloseCreateRideDetailMessage<RideWrapper>>(CloseCreateRideDetail);
            _mediator.Register<SignOutMessage<UserWrapper>>(SignOut);
            _mediator.Register<OpenCarDetailMessage<CarWrapper>>(OpenCarDetail);
            _mediator.Register<CloseCarDetailMessage<CarWrapper>>(CloseCarDetail);
            _mediator.Register<CloseUserMenuMessage<UserWrapper>>(CloseUserMenu);
            _mediator.Register<OpenAddressDetailMessage<AddressWrapper>>(OpenAddressDetail);
            _mediator.Register<CloseAddressDetailMessage<AddressWrapper>>(CloseAddressDetail);

            // Register other messages
            _mediator.Register<SelectedMessage<UserWrapper>>(StoreLoggedInUser);
            _mediator.Register<SelectedMessage<RideWrapper>>(OnRideSelected);

        }

        // ViewModel properties
        public IFilterRidesViewModel FilterRidesViewModel { get; }
        public IRideListViewModel RideListViewModel { get; }
        public IRideDetailViewModel RideDetailViewModel { get; }
        public ICreateRideViewModel CreateRideViewModel { get; }
        public IUserMenuViewModel UserMenuViewModel { get; }
        public ICarDetailViewModel CarDetailViewModel { get; }
        public ICarListViewModel CarListViewModel { get; }
        public IEditCarViewModel EditCarViewModel { get; }
        public IAddressDetailViewModel AddressDetailViewModel { get; }
        public UserWrapper? LoggedInUserModel { get; private set; }

        public ICommand OpenUserMenuCommand { get; }
        public ICommand CloseUserMenuCommand { get; }

        // View visibility properties
        public bool FilterRidesViewVisibility { get; set; }
        public bool RideListViewVisibility { get; set; }
        public bool RideDetailViewVisibility { get; set; }
        public bool CreateRideViewVisibility { get; set; }
        public bool UserMenuViewVisibility { get; set; }
        public bool CarDetailViewVisibility { get; set; }
        public bool EditCarViewVisibility { get; set; }
        public bool AddressDetailViewVisibility { get; set; }

        private void InitializeView(OpenRideWithMeViewMessage<UserWrapper> _)
        {
            _mediator.Register<EditUserMessage<UserWrapper>>(UpdateUserMenu);
            // Set initial view visibilities
            RideListViewVisibility = true;
            FilterRidesViewVisibility = true;
            EditCarViewVisibility = true;
            RideDetailViewVisibility = false;
            CreateRideViewVisibility = false;
            UserMenuViewVisibility = false;
            CarDetailViewVisibility = false;
        }

        private void CloseRideDetail(CloseRideDetailMessage<RideWrapper> _)
        {
            RideDetailViewVisibility = false;
        }

        private void OpenCreateRideDetail(OpenCreateRideDetailMessage<RideWrapper> _)
        {
            CreateRideViewVisibility = true;
            FilterRidesViewVisibility = false;
            RideListViewVisibility = false;
            RideDetailViewVisibility = false;
            UserMenuViewVisibility = false;
            EditCarViewVisibility = false;
        }

        private void CloseCarDetail(CloseCarDetailMessage<CarWrapper> _)
        {
            CarDetailViewVisibility = false;
            FilterRidesViewVisibility = true;
            RideListViewVisibility = true;

            _mediator.Send(new UpdateMessage<CarWrapper>());
        }

        private void OpenCarDetail(OpenCarDetailMessage<CarWrapper> carMessage)
        {
            CreateRideViewVisibility = false;
            FilterRidesViewVisibility = false;
            RideListViewVisibility = false;
            RideDetailViewVisibility = false;
            UserMenuViewVisibility = false;
            CarDetailViewVisibility = true;

            CarDetailViewModel.LoadAsync(carMessage.Id ?? Guid.Empty);
        }
        private void OpenAddressDetail(OpenAddressDetailMessage<AddressWrapper> _)
        {
            CreateRideViewVisibility = false;
            FilterRidesViewVisibility = false;
            RideListViewVisibility = false;
            RideDetailViewVisibility = false;
            UserMenuViewVisibility = false;
            CarDetailViewVisibility = false;

            AddressDetailViewVisibility = true;

            CarDetailViewModel.LoadAsync(Guid.Empty);
        }
        private void CloseAddressDetail(CloseAddressDetailMessage<AddressWrapper> _)
        {
            CreateRideViewVisibility = true;
            AddressDetailViewVisibility = false;

            CarDetailViewModel.LoadAsync(Guid.Empty);
        }

        private void CloseCreateRideDetail(CloseCreateRideDetailMessage<RideWrapper> _)
        {
            RideListViewVisibility = true;
            FilterRidesViewVisibility = true;
            CreateRideViewVisibility = false;
            RideDetailViewVisibility = false;
            EditCarViewVisibility = true;
        }

        private async void StoreLoggedInUser(SelectedMessage<UserWrapper> _)
        {
            var loggedUserId = _loggedInUserService.GetLoggedUserGuid();
            LoggedInUserModel = await _userFacade.GetAsync(loggedUserId);
        }

        private async void UpdateUserMenu(EditUserMessage<UserWrapper> _)
        {
            var loggedUserId = _loggedInUserService.GetLoggedUserGuid();
            LoggedInUserModel = await _userFacade.GetAsync(loggedUserId);
        }

        private void OnRideSelected(SelectedMessage<RideWrapper> message)
        {
            RideDetailViewVisibility = true;
            SelectRide(message.Id);
        }

        private void SelectRide(Guid? id)
        {
            if (id != null)
            {
                RideDetailViewModel.LoadAsync(id.Value);
                _mediator.Send(new NewMessage<RideWrapper>());
            }
        }

        private void OnOpenUserMenu()
        {
            UserMenuViewVisibility = true;
        }

        private void OnCloseUserMenu() => _mediator.Send(new CloseUserMenuMessage<UserWrapper>());

        private void CloseUserMenu(CloseUserMenuMessage<UserWrapper> _)
        {
            UserMenuViewVisibility = false;
        }

        private void SignOut(SignOutMessage<UserWrapper> _)
        {
            RideListViewVisibility = false;
            FilterRidesViewVisibility = false;
            RideDetailViewVisibility = false;
            CreateRideViewVisibility = false;
            UserMenuViewVisibility = false;
        }
    }
}
