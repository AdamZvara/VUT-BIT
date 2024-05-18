using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using Microsoft.IdentityModel.Protocols.OpenIdConnect;
using Microsoft.Toolkit.Mvvm.Input;
using RideWithMe.App.Factories;
using RideWithMe.App.Messages;
using RideWithMe.App.Messages.ViewMessages;
using RideWithMe.App.Services;
using RideWithMe.App.ViewModels.Interfaces;
using RideWithMe.App.Wrappers;
using RideWithMe.BL.Models;

namespace RideWithMe.App.ViewModels
{
    public class MainViewModel : ViewModelBase
    {
        private readonly IMediator _mediator;

        public MainViewModel(
            IMediator mediator,
            IUserListViewModel userListViewModel,
            IUserDetailViewModel userDetailViewModel,
            IRideWithMeViewModel rideWithMeViewModel)
        {
            UserListViewModel = userListViewModel;
            RideWithMeViewModel = rideWithMeViewModel;
            UserDetailViewModel = userDetailViewModel;

            RefreshCommand = new RelayCommand(SendRefreshMessage);

            _mediator = mediator;

            // Set initial states of views
            UserListViewVisible = true;
            UserDetailViewVisible = false;
            RideWithMeViewVisible = false;

            // Register view visibility messages
            mediator.Register<OpenUserDetailMessage<UserWrapper>>(OpenUserDetail);
            mediator.Register<CloseUserDetailMessage<UserWrapper>>(CloseUserDetail);
            mediator.Register<SelectedMessage<UserWrapper>>(OpenRideWithMeView);
            mediator.Register<SignOutMessage<UserWrapper>>(SignOut);

        }

        // Properties representing ViewModels
        public IUserListViewModel UserListViewModel { get; }
        public IUserDetailViewModel UserDetailViewModel { get; }
        public IRideWithMeViewModel RideWithMeViewModel { get; }
        public ICommand RefreshCommand { get; }

        // Boolean properties representing visibility of views
        public bool UserDetailViewVisible { get; set; }
        public bool UserListViewVisible { get; set; }
        public bool RideWithMeViewVisible { get; set; }

        private void SendRefreshMessage()
        {
            _mediator.Send(new RefreshMessage<RideWrapper> { });
            _mediator.Send(new RefreshMessage<UserWrapper> { });
            _mediator.Send(new RefreshMessage<AddressWrapper> { });


        }
        private void OpenUserDetail(OpenUserDetailMessage<UserWrapper> message)
        {
            UserListViewVisible = false;
            RideWithMeViewVisible = false;
            UserDetailViewVisible = true;

            UserDetailViewModel.LoadAsync(message.Id ?? Guid.Empty);

            if (message.Id != null) {
                _mediator.UnRegister<CloseUserDetailMessage<UserWrapper>>(CloseUserDetail);
                _mediator.Register<CloseUserDetailMessage<UserWrapper>>(CloseUserEditDetail);
            }
        }

        private void CloseUserDetail(CloseUserDetailMessage<UserWrapper> _)
        {
            UserDetailViewVisible = false;
            RideWithMeViewVisible = false;
            UserListViewVisible = true;
        }

        private void CloseUserEditDetail(CloseUserDetailMessage<UserWrapper> _)
        {
            UserDetailViewVisible = false;
            UserDetailViewVisible = false;
            RideWithMeViewVisible = true;
        }

        private void OpenRideWithMeView(SelectedMessage<UserWrapper> _)
        {
            UserDetailViewVisible = false;
            UserListViewVisible = false;
            RideWithMeViewVisible = true;

            _mediator.Send(new OpenRideWithMeViewMessage<UserWrapper>());
        }

        private void SignOut(SignOutMessage<UserWrapper> _)
        {
            UserListViewVisible = true;
            RideWithMeViewVisible = false;

            _mediator.UnRegister<CloseUserDetailMessage<UserWrapper>>(CloseUserEditDetail);
            _mediator.Register<CloseUserDetailMessage<UserWrapper>>(CloseUserDetail);
        }

    }
}
